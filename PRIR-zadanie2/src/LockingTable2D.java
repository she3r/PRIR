public class LockingTable2D{
    private final Table2D table;
    private final Integer[][] acquiredTable;
    private final Object[][] locks;
    public final int rows;
    public final int cols;

    LockingTable2D(Table2D table) {
        this.table = table;
        this.rows = table.rows();
        this.cols = table.cols();
        this.locks = new Object[rows][cols];
        this.acquiredTable = new Integer[rows][cols];
    }
    // try set to worktable and get val
    public int AcquireNotifyValue(int row, int column){
        synchronized(locks[row][column]){
            if(acquiredTable[row][column] != null)
                return acquiredTable[row][column];
            var result = table.get(new Position2D(column, row));
            acquiredTable[row][column] = result;
            locks[row][column].notify();
        }
        return acquiredTable[row][column];
    }
    public int AcquireValue(int row, int column){
        //synchronized(locks[row][column]){
            if(acquiredTable[row][column] != null)
                return acquiredTable[row][column];
            var result = table.get(new Position2D(column, row));
            acquiredTable[row][column] = result;
        //}
        return acquiredTable[row][column];
    }
    public int AcquireValue(Position2D position2D){
        return AcquireValue(position2D.row(), position2D.col());
    }
    public int AcquireNotifyValue(Position2D position2D){
        return AcquireNotifyValue(position2D.row(), position2D.col());
    }
    public Integer GetValue(int row, int column){
        return acquiredTable[row][column];
    }
    public Integer GetValue(Position2D position2D){
        return GetValue(position2D.row(),position2D.col());
    }
    public Integer GetWaitValue(int row, int column) throws InterruptedException{
        synchronized (locks[row][column]){
            while(acquiredTable[row][column] == null)
                locks[row][column].wait();
            return acquiredTable[row][column];
        }
    }
    public Integer GetWaitValue(Position2D position2D) throws InterruptedException{
        return GetWaitValue(position2D.row(),position2D.col());
    }
}