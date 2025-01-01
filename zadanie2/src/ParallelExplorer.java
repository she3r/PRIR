import java.util.LinkedList;
import java.util.Queue;
import java.util.Set;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

record Tuple<A, B>(A first, B second) {
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
        acquiredTable = new Integer[rows][cols];
    }

    public Tuple<Integer, Boolean> AcquireValue(int row, int column){
        if(row >= rows || column >= cols){
            return null;
        }
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
    private MoveDirection moveDirection;
    private ThreadAndPosition[] allReadThreads;
    private Thread writerThread;
    private int sumToFind;

    @Override
    public void setThreadsFactory(ThreadsFactory factory) {
        this.factory = factory;
        allReadThreads = new ThreadAndPosition[factory.readersThreads()];
        isThreadDone = new boolean[factory.readersThreads()];
    }

    private Runnable GetProducer(int threadId){
        return () -> {
            var startPos = allReadThreads[threadId].position();
            Queue<Position2D> positionsToVisit = new LinkedList<>();
            positionsToVisit.add(startPos);

            while(!positionsToVisit.isEmpty()){
                var currToVisit = positionsToVisit.poll();
                var baseValue = workingTable.AcquireValue(currToVisit).first();
                // sprawdzamy jedynie relacje srodek- osmiu sasiadow.
                // Reszta zostanie rozpoznana w innych iteracjach (mozliwe ze w innych watkach)
                for(Position2D neighbourPosition : PositionHelper.GetAllNeighbours(currToVisit)){
                    var acquireResult = workingTable.AcquireValue(neighbourPosition);
                    if(acquireResult == null)
                        // przekroczono zakres tablicy - nie ma takiego sasiada
                        continue;
                    if(acquireResult.second())   // jesli to tym watkiem w tej iteracji przeczytano wartosc
                        positionsToVisit.add(neighbourPosition);
                    if(acquireResult.first() + baseValue == sumToFind)
                    {
                        try {
                            //toZeroWriteQueue.add(new Pair(currToVisit, neighbourPosition));
                            toZeroWriteQueue.put(new Pair(currToVisit, neighbourPosition));
                        } catch (InterruptedException e) {
                            throw new RuntimeException(e);
                        }
                    }
                }
            }
            // wyslij info ze ten watek skonczyl
            try {
                //toZeroWriteQueue.add(new Pair(new Position2D(finalI,finalI), new Position2D(Integer.MIN_VALUE, Integer.MIN_VALUE)));
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
            while(true){
                try {
                    var item = toZeroWriteQueue.take();
                    if(IsTerminalPair(item)){
                        var threadNum = item.first();
                        if(threadNum.row() != threadNum.col())
                            throw new IllegalArgumentException("zły format wpisu terminalnego - niejednoznaczny numer watku");
                        if(SetThreadAsDoneAndCheckIfAllDone(threadNum.col()))
                            break;
                    }
                    // zeruj na tablicy wejsciowej. Dodaj do zbioru wynikowego
                    originalTable.set0(item.first());
                    originalTable.set0(item.second());
                    results.add(item);
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
        for(int i=0;i< factory.readersThreads();++i){
            var producer = GetProducer(i);
            allReadThreads[i] = factory.readerThread(producer);
        }
        var consumer = GetConsumer();
        writerThread = factory.writterThread(consumer);
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
