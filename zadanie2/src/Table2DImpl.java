public class Table2DImpl implements Table2D {
    private final int[][] table;

    public Table2DImpl(int[][] table2d) {
        table = table2d;
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

    // Metoda do ustawiania wartości w tablicy (opcjonalna, dla ułatwienia testowania)
    public void set(Position2D position, int value) {
        table[position.row()][position.col()] = value;
    }
}