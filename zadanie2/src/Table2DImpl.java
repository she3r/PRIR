public class Table2DImpl implements Table2D {
    private final int[][] table;
    private final int[][] usageTable;

    public Table2DImpl(int[][] table2d) {
        table = table2d;
        usageTable = new int[table.length][table[0].length];
    }

    @Override
    public int cols() {
        return table[0].length;
    }

    @Override
    public int rows() {
        return table.length;
    }

    @Override
    public int get(Position2D position) {
//        try {
//            Thread.sleep(5);
//        } catch (InterruptedException e) {
//            throw new RuntimeException(e);
//        }
        usageTable[position.row()][position.col()]++;
        return table[position.row()][position.col()];
    }

    @Override
    public void set0(Position2D position) {
        table[position.row()][position.col()] = 0;
    }

    // Metoda, która umożliwia ładne drukowanie tablicy 2D
    public void prettyPrint() {
        for (int[] row : table) {
            for (int value : row) {
                System.out.print(value + "\t");
            }
            System.out.println();
        }
    }

    public void AssertLegalUsage() throws Exception {
        for(int i = 0; i < usageTable.length; i++) {
            for (int j = 0; j < table[0].length; j++) {
                if(table[i][j] == 1){
                    throw new Exception(String.format("brak zera w tablicy. Wartosc=%d. Row=%d, Col=%d", usageTable[i][j], i, j));
                }
                if (usageTable[i][j] != 1) {
                    throw new Exception(String.format("nielegalne wykorzystanie tablicy. Wartosc=%d. Row=%d, Col=%d", usageTable[i][j], i, j));
                }
            }
        }
    }
}
