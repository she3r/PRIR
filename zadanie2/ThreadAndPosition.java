/**
 * Para wątek oraz jego położenie startowe.
 * 
 * @param thread wątek uprawniony do odczytu danych z tablicy
 * @param position pierwsza pozycja do odczytu danych z tablicy dla wątku thread
 */
public record ThreadAndPosition( Thread thread, Position2D position ) {

}