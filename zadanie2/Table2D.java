/**
 * Interfejs dwu-wymiarowej tablicy liczb całkowitych. Poprawe pozycje w tablicy
 * to: <br>
 * <ul>
 * <li>Dla wierszy od 0 do rows() - 1
 * <li>Dla kolumn od 0 do cols() - 1
 * </ul>
 */
public interface Table2D {
	/**
	 * Metoda zwraca liczbę kolumn w tablicy.
	 * 
	 * @return liczba kolumn
	 */
	int cols();

	/**
	 * Metoda zwraca liczbę wierszy w tablicy.
	 * 
	 * @return liczba wierszy
	 */
	int rows();

	/**
	 * Wartość liczbowa zapisana w tablicy na pozycji position.
	 * 
	 * @param position położenie do odczytu
	 * @return wartość liczbowa zapisana w tablicy
	 */
	int get(Position2D position);

	/**
	 * Wpisuje do tablicy 0 na pozycji position.
	 * 
	 * @param position położenie, w którym należy zapisać w tablicy 0
	 */
	void set0(Position2D position);
}
