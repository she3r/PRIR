import java.util.HashSet;
import java.util.Random;
import java.util.Set;

public class Main {
    public static void main(String[] args) throws Exception {
        int numTests = 1000;
        for(int i=0;i<numTests;i++) {
            DoTest();
        }
    }

    private static void DoTest() throws Exception {
                // 10 x 100
//        var innerTable2d = new int[][]{
//                {1,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
//                {1,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,5,0,0,0,0,0,0,0,0,0,0,0},
//                {5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//                {0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,2,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,9,9,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//                {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//                {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,1,1},
//
//        };
        var size1=3000;
        var size2=1000;
        var innerTable2d = new int[size1][size2];
        for (int i = 0; i < size1; i++)
            for (int j = 0; j < size2; j++)
                innerTable2d[i][j] = 0;

        Random random = new Random();
        //innerTable2d[15][15] = 1; <- sprawdzenie czy ja zostawi (rzuci wyjatkiem w assercie na koncu)
        for (int row = 1; row < size1-1; row += random.nextInt(3,10)) { // Wstawianie co 10 wierszy
            for (int col = 1; col < size2-1; col += random.nextInt(3,10)) {
                innerTable2d[row - 1][col - 1] = random.nextInt(2,10);
                innerTable2d[row - 1][col] = random.nextInt(2,10);
                innerTable2d[row - 1][col + 1] = random.nextInt(2,10);
                innerTable2d[row][col - 1] = random.nextInt(2,10);
                innerTable2d[row][col] = random.nextInt(2,10);
                innerTable2d[row][col + 1] = random.nextInt(2,10);
                innerTable2d[row + 1][col - 1] = random.nextInt(2,10);
                innerTable2d[row + 1][col] = random.nextInt(2,10);
                innerTable2d[row + 1][col + 1] = random.nextInt(2,10);
                // te dwa maja sie znalezc
                innerTable2d[row][col] = 1;
                innerTable2d[row][col + 1] = 1;
            }
        }
        var table2d = new Table2DImpl(innerTable2d);
        var explorer = getParallelExplorer(innerTable2d, table2d);
        explorer.start(2);
        table2d.prettyPrint();
        while(explorer.result().isEmpty()){
            Thread.sleep(5);
        }
        System.out.println("Result");
        for(var r : explorer.result()){
            System.out.printf("row=%d col=%d , row=%d col=%d%n", r.first().row(),r.first().col(), r.second().row(), r.second().col());
        }
        table2d.AssertLegalUsage();
        //table2d.prettyPrint();
    }

    private static Position2D[] generateUniquePairs(int N, int M, int totalPairs) {
        Set<Position2D> uniquePairs = new HashSet<>();
        Random random = new Random();

        while (uniquePairs.size() < totalPairs) {
            int x = random.nextInt(N);
            int y = random.nextInt(M);
            uniquePairs.add(new Position2D(y,x));
        }

        return  uniquePairs.toArray(new Position2D[0]);
    }

    private static ParallelExplorer getParallelExplorer(int[][] innerTable2d, Table2DImpl table2d) {
        int rows = innerTable2d.length;
        int cols = innerTable2d[0].length;
//        var positions = new Position2D[]{
//                new Position2D(0, 0),
//                new Position2D(cols-1, 0),
//                new Position2D(0, rows-1),
//                new Position2D(cols-1, rows-1),
//                new Position2D(5,5),
//                new Position2D(6,6),
//                new Position2D(7,7),
//                new Position2D(9,5),
//        };
        var positions = generateUniquePairs(rows,cols, 30);
        var factory = new ExtendedLoggingThreadsFactory(positions.length, positions);
        var explorer = new ParallelExplorer();
        explorer.setThreadsFactory(factory);
        explorer.setTable(table2d);
        return explorer;
    }
}