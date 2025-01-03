public class Main {
    public static void main(String[] args) throws InterruptedException {
        var innerTable2d = new int[][]{
                {1,1,0,0,1},
                {0,0,0,1,0},
                {0,0,0,0,0},
                {0,1,0,0,1},
                {1,0,0,0,1},
        };

        var table2d = new Table2DImpl(innerTable2d);
        var explorer = getParallelExplorer(innerTable2d, table2d);
        table2d.prettyPrint();
        while(explorer.result().isEmpty()){
            Thread.sleep(5);
        }
        System.out.println("Result");
        for(var r : explorer.result()){
            System.out.printf("%d %d , %d %d%n", r.first().row(),r.first().col(), r.second().row(), r.second().col());
        }
        table2d.prettyPrint();

    }

    private static ParallelExplorer getParallelExplorer(int[][] innerTable2d, Table2DImpl table2d) {
        int rows = innerTable2d.length;
        int cols = innerTable2d[0].length;
        var positions = new Position2D[]{
                new Position2D(0, 0),
                new Position2D(rows-1, 0),
                new Position2D(0, cols-1),
                new Position2D(rows-1, cols-1),
        };
        var factory = new ExtendedLoggingThreadsFactory(positions.length, positions);
        var explorer = new ParallelExplorer();
        explorer.setTable(table2d);
        explorer.setThreadsFactory(factory);
        explorer.start(2);
        return explorer;
    }
}