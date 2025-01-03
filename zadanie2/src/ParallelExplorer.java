import java.util.LinkedList;
import java.util.Queue;
import java.util.Set;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

record Tuple<A, B>(A first, B second) {
}

class PositionHelper{
    private static Position2D GetRightUpper(Position2D position2D){
        return new Position2D(position2D.col() + 1, position2D.row() + 1);
    }
    private static Position2D GetLeftLower(Position2D position2D){
        return new Position2D(position2D.col() - 1, position2D.row() - 1);
    }
    private static Position2D GetRightLower(Position2D position2D){
        return new Position2D(position2D.col() + 1, position2D.row() - 1);
    }
    private static Position2D GetLeftUpper(Position2D position2D){
        return new Position2D(position2D.col() - 1, position2D.row() + 1);
    }
    private static Position2D GetLowMid(Position2D position2D){
        return new Position2D(position2D.col(), position2D.row() - 1);
    }
    private static Position2D GetHighMid(Position2D position2D){
        return new Position2D(position2D.col(), position2D.row() + 1);
    }
    private static Position2D GetRightMid(Position2D position2D){
        return new Position2D(position2D.col() + 1, position2D.row());
    }
    private static Position2D GetLeftMid(Position2D position2D){
        return new Position2D(position2D.col() - 1, position2D.row());
    }

    public static Position2D[] GetAllNeighbours(Position2D position2D){
        return new Position2D[]{GetLowMid(position2D), GetRightLower(position2D), GetRightMid(position2D),
                GetRightUpper(position2D), GetHighMid(position2D), GetLeftUpper(position2D),
                GetLeftMid(position2D), GetLeftLower(position2D)};
    }
}

class LockingTable2D{
    private final Table2D table;
    private final Integer[][] acquiredTable;
    private final Object[][] locks;
    private final int rows;
    private final int cols;

    LockingTable2D(Table2D table2d) {
        this.table = table2d;
        rows = table2d.rows();
        cols = table2d.cols();
        locks = new Object[rows][cols];
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                locks[i][j] = new Object();

        acquiredTable = new Integer[rows][cols];
    }

    public Tuple<Integer, Boolean> AcquireValue(int row, int column){
        if(row < 0 || row >= rows || column < 0 ||column >= cols)
            return null;

        synchronized(locks[row][column]){
            // to musi byc w locku: nie odczytujemy od razu bo jakis watek moze byc w trakcie zapisywania tego elementu
            if(acquiredTable[row][column] != null)
                return new Tuple<>(acquiredTable[row][column], false);
            var result = table.get(new Position2D(column, row));
            acquiredTable[row][column] = result;
            // wartosc zostala odczytana i to ten watek odczytal
            return new Tuple<>(acquiredTable[row][column], true);
        }
    }
    public Tuple<Integer, Boolean> AcquireValue(Position2D position2D){
        return AcquireValue(position2D.row(), position2D.col());
    }
}


public class ParallelExplorer implements Explorer{
    private ThreadsFactory factory;
    private Table2D originalTable;
    private LockingTable2D workingTable;
    private BlockingQueue<Pair> toZeroWriteQueue;
    private boolean[] isThreadDone;
    private ThreadAndPosition[] allReadThreads;
    private Thread writerThread;
    private int sumToFind;

    @Override
    public void setThreadsFactory(ThreadsFactory factory) {
        this.factory = factory;
        allReadThreads = new ThreadAndPosition[factory.readersThreads()];
        isThreadDone = new boolean[factory.readersThreads()];
        for(int i=0;i< factory.readersThreads();++i){
            var producer = GetProducer(i);
            allReadThreads[i] = this.factory.readerThread(producer);
        }
        var consumer = GetConsumer();
        writerThread = this.factory.writterThread(consumer);
    }

    private Runnable GetProducer(int threadId){
        return () -> {
            var startPos = allReadThreads[threadId].position();
            Queue<Position2D> positionsToVisit = new LinkedList<>();
            positionsToVisit.add(startPos);

            while(!positionsToVisit.isEmpty()){
                var currToVisit = positionsToVisit.poll();
                var acquired = workingTable.AcquireValue(currToVisit);
                if(acquired == null)
                    throw new NullPointerException(String.format("reading null base value. Row=%d Col=%d",currToVisit.row(),currToVisit.col()));
                var baseValue = acquired.first();
                // sprawdzamy jedynie relacje srodek- osmiu sasiadow.
                // Reszta zostanie rozpoznana w innych iteracjach (mozliwe ze w innych watkach)
                for(Position2D neighbourPosition : PositionHelper.GetAllNeighbours(currToVisit)){
                    var acquireResult = workingTable.AcquireValue(neighbourPosition);
                    if(acquireResult == null)
                        // przekroczono zakres tablicy - nie ma takiego sasiada
                        continue;
                    if(acquireResult.second())
                        // jesli to tym watkiem w tej iteracji przeczytano wartosc
                        positionsToVisit.add(neighbourPosition);
                    if(acquireResult.first() + baseValue == sumToFind)
                    {
                        try {
                            toZeroWriteQueue.put(new Pair(currToVisit, neighbourPosition));
                        } catch (InterruptedException e) {
                            throw new RuntimeException(e);
                        }
                    }
                }
            }
            // wyslij info ze ten watek skonczyl
            try {
                toZeroWriteQueue.put(new Pair(new Position2D(threadId,threadId), terminalPosition));
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        };
    }

    private final Position2D terminalPosition = new Position2D(Integer.MIN_VALUE, Integer.MIN_VALUE);

    private Runnable GetConsumer(){
        return () -> {
            Set<Pair> results = new java.util.HashSet<>(Set.of());
            boolean isDone = false;
            while(true){
                try {
                    if(isDone && toZeroWriteQueue.isEmpty())
                        break;
                    var item = toZeroWriteQueue.take();
                    if(IsTerminalPair(item)){
                        var threadNum = item.first();
                        if(threadNum.row() != threadNum.col())
                            throw new IllegalArgumentException("zły format wpisu terminalnego - niejednoznaczny numer watku");
                        if(SetThreadAsDoneAndCheckIfAllDone(threadNum.col()))
                            isDone=true;
                        else continue;
                    }
                    else {
                        // zeruj na tablicy wejsciowej. Dodaj do zbioru wynikowego
                        originalTable.set0(item.first());
                        originalTable.set0(item.second());
                        results.add(item);
                    }
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
            finalResult = results;
        };
    }

    @Override
    public void setTable(Table2D table) {
        originalTable = table;
        toZeroWriteQueue  = new ArrayBlockingQueue<>(originalTable.cols() * originalTable.rows(), true);
        this.workingTable = new LockingTable2D(table);
    }

    @Override
    public void start(int sum) {
        sumToFind = sum;
        for(var thread : allReadThreads)
            thread.thread().start();
        writerThread.start();
    }

    private Set<Pair> finalResult = Set.of();
    @Override
    public Set<Pair> result() {
        return finalResult;
    }

    private boolean IsTerminalPair(Pair pair){
        var potentialTerminal = pair.second();
        return potentialTerminal.row() == terminalPosition.row() && potentialTerminal.col() == terminalPosition.col();
    }

    private boolean SetThreadAsDoneAndCheckIfAllDone(int threadNum){
        if(threadNum >= isThreadDone.length)
            throw new IllegalArgumentException("niepoprawny numer watku do oznaczenia jako terminowany");
        isThreadDone[threadNum] = true;
        for (boolean value : isThreadDone) if (!value) return false;
        return true;
    }
}