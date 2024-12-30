/**
 * Interfejs fabryki wątków
 */
public interface ThreadsFactory {
	/**
	 * Liczba wątków z uprawnieniami od odczytu danych.
	 * 
	 * @return liczba wątków
	 */
	int readersThreads();

	/**
	 * Metoda zwraca obiekt-wątek w stanie NEW (nieuruchomiony). Wątki zwracane są w
	 * parze z początkową pozycją tablicy, od której powinny rozpocząć odczyt
	 * danych. Po odebraniu readersThreads wątków kolejne wywołanie zwróci null.
	 * 
	 * @param run kod do wykonania w wątku
	 * @return wątek z prawami do odczytu i jego pozycja startowa
	 */
	ThreadAndPosition readerThread(Runnable run);

	/**
	 * Metoda zwraca obiekt-wątek w stanie NEW. Wątek zwrócony przez tą metodę jako
	 * jedyny ma prawo zmieniać stan tablicy z danymi. Wszystkie wywołania metody
	 * będą zwracać ten sam jeden wątek.
	 * 
	 * @param run kod do wykonania w wątku
	 * @return wątek z prawami do zapisu
	 */
	Thread writterThread(Runnable run);
}
