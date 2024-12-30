import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.Set;
import java.util.concurrent.ConcurrentLinkedQueue;


public class ParallelExplorer implements Explorer{
    private ThreadsFactory factory;
    private LockingTable2D workingTable;
    private final ConcurrentLinkedQueue<Pair> toZeroWriteQueue = new ConcurrentLinkedQueue<>();
    private MoveDirection moveDirection;
    private ThreadAndPosition[] allReadThreads;
    private Thread writerThread;
    private int[][] leftRightBorder;    // indeks watku: left, right
    private int sumToFind;
    private Tuple<Integer, Position2D>[] threadStartingPositinsOrdered;
    @Override
    public void setThreadsFactory(ThreadsFactory factory) {
        this.factory = factory;
    }


    // add unregister value to queue
    private void UnregisterThread(int threadNum){
        toZeroWriteQueue.add(new Pair(new Position2D(Integer.MIN_VALUE, Integer.MIN_VALUE), new Position2D(threadNum,threadNum)));
    }

    private Runnable GetProducer(int currIndex){
        final int numOfRows = workingTable.rows;
        final int numOfCols = workingTable.cols;
        final Position2D startingPosition = allReadThreads[currIndex].position();
        final int borderColumnsLeft = leftRightBorder[currIndex][0];
        final int borderColumnsRight = leftRightBorder[currIndex][0];
        final boolean isLastThread = (leftRightBorder[currIndex][1] == numOfCols - 1);
        final boolean isFirstThread = (leftRightBorder[currIndex][0] == 0);
        final boolean isAllowedToAcquireFirstRightColumnBeyondBorder;
        Position2D prevOrderedThreadStartPosition = null;

        if(isLastThread) isAllowedToAcquireFirstRightColumnBeyondBorder = false;
        else isAllowedToAcquireFirstRightColumnBeyondBorder = (leftRightBorder[currIndex+1][1] - leftRightBorder[currIndex+1][0] > 1);
        if(!isFirstThread)
            for(int i=0;i<threadStartingPositinsOrdered.length;i++)
                if(threadStartingPositinsOrdered[i].getSecond() == startingPosition)
                    prevOrderedThreadStartPosition = threadStartingPositinsOrdered[i-1].getSecond();

        final Position2D finalPrevOrderedThreadStartPosition = prevOrderedThreadStartPosition;
        return () -> {
                var currPos = startingPosition;
                var moveDirection = MoveDirection.Up;
                while(true){
                    boolean isCrossingRightBorder = borderColumnsRight == currPos.col();
//                    boolean isAllowedToAcquireFirstLeftColumn = !isFirstThread
//                            && (currPos.col() == borderColumnsLeft + 1
//                            && currPos.row() < finalPrevOrderedThreadStartPosition.row());

                    ExploringMode leftExploringMode;
                    if(!isFirstThread && currPos.col() == borderColumnsLeft + 1){
                        if(currPos.row() < finalPrevOrderedThreadStartPosition.row())
                            leftExploringMode = ExploringMode.Acquire;
                        else leftExploringMode = ExploringMode.GetWait;
                    }
                    else leftExploringMode = ExploringMode.Get;
                    CellNeighbourhood curNeighbourhood = null;
                    try {
                        curNeighbourhood = PositionHelper.GetCurrNeighbourhood(currPos, workingTable,
                                isAllowedToAcquireFirstRightColumnBeyondBorder || !isCrossingRightBorder,
                                leftExploringMode, moveDirection);
                    } catch (InterruptedException e) {
                        throw new RuntimeException(e);
                    }
                    var hits = PositionHelper.FindPairs(workingTable, currPos, curNeighbourhood, sumToFind);
                    toZeroWriteQueue.addAll(hits);

                    currPos = switch (moveDirection) {
                        case Up -> {
                            if (numOfRows - 1 == currPos.row()) {
                                moveDirection = MoveDirection.Down;
                                yield new Position2D(currPos.col() - 1, numOfRows - 1);
                            }
                            yield new Position2D(currPos.col(), currPos.row() + 1);
                        }
                        case Down -> {
                            if (0 == currPos.row()) {
                                moveDirection = MoveDirection.Up;
                                yield new Position2D(currPos.col() - 1, 0);
                            }
                            yield new Position2D(currPos.col(), currPos.row()-1);
                        }
                    };
                    if(currPos.col() < borderColumnsLeft){
                        UnregisterThread(currIndex);
                        return;
                    }
                }

            };
    }

    @Override
    public void setWorkingTable(Table2D table2d) {
        allReadThreads = new ThreadAndPosition[factory.readersThreads()];
        threadStartingPositinsOrdered = new Tuple[factory.readersThreads()];
        for(int i=0;i< factory.readersThreads();++i){
            allReadThreads[i] = factory.readerThread(GetProducer(i));
            threadStartingPositinsOrdered[i] = new Tuple<>(i, allReadThreads[i].position());
        }
        Arrays.sort(threadStartingPositinsOrdered, Comparator.comparingInt(o -> o.getSecond().col()));

        leftRightBorder = new int[factory.readersThreads()][2];
        for(int i=0;i<factory.readersThreads();++i){
            if(i == 0)
                leftRightBorder[threadStartingPositinsOrdered[i].getFirst()] = new int[]{0, threadStartingPositinsOrdered[i].getSecond().col()};
            else if(i == factory.readersThreads() - 1)
                leftRightBorder[threadStartingPositinsOrdered[i].getFirst()]
                        = new int[]{threadStartingPositinsOrdered[i - 1].getSecond().col() + 1, table2d.cols() - 1};
            else
                leftRightBorder[threadStartingPositinsOrdered[i].getFirst()]
                        = new int[]{threadStartingPositinsOrdered[i - 1].getSecond().col() + 1, threadStartingPositinsOrdered[i].getSecond().col()};
        }
        Runnable consumer = () -> {

        };
        writerThread = factory.writterThread(consumer);
        this.workingTable = new LockingTable2D(table2d);
    }

    @Override
    public void start(int sum) {
        sumToFind = sum;
    }

    @Override
    public Set<Pair> result() {
        return Set.of();
    }
}

class Tuple<A, B> {
    private final A first;
    private final B second;

    public Tuple(A first, B second) {
        this.first = first;
        this.second = second;
    }

    public A getFirst() {
        return first;
    }

    public B getSecond() {
        return second;
    }
}
