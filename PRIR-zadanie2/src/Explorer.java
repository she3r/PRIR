import java.util.Set;

/**
 * Główny interfejs programu poszukującego sum w tablicy.
 */
public interface Explorer {
	/**
	 * Ustawienie fabryki wątków
	 * 
	 * @param factory fabryka wątków
	 */
	void setThreadsFactory(ThreadsFactory factory);

	/**
	 * Ustawienie dostępu do tablicy z danymi.
	 * 
	 * @param workingTable tablica z danymi
	 */
	void setWorkingTable(Table2D workingTable);

	/**
	 * Start poszukiwania sąsiednich pozycji zawierających łączenie wartość sum.
	 * 
	 * @param sum poszukiwana suma
	 */
	void start(int sum);

	/**
	 * Wynik działania programu. W trakcie pracy programu metoda zwraca pusty zbiór.
	 * 
	 * @return zbiór z parami sąsiednich położeń, których suma dała wartość sum.
	 */
	Set<Pair> result();
}
