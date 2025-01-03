import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.logging.Level;  
import java.util.logging.Logger;  
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.Executors;

public class ExtendedLoggingThreadsFactory implements ThreadsFactory {  
  
    public static final Logger logger = Logger.getLogger(ExtendedLoggingThreadsFactory.class.getName());
    private final int totalReaders;  
    private final AtomicInteger currentReaderIndex = new AtomicInteger(0);  
    private Thread writerThread;  
    private final Object writerLock = new Object();
    private final Position2D[] threadStartingPositions;

    public ExtendedLoggingThreadsFactory(int totalReaders, Position2D[] threadStartingPositions1) {
        this.totalReaders = totalReaders;
        this.threadStartingPositions = threadStartingPositions1;
    }
  
    @Override  
    public int readersThreads() {  
        return totalReaders;  
    }  
  
    @Override  
    public ThreadAndPosition readerThread(Runnable run) {  
        int index = currentReaderIndex.getAndIncrement();  
        if (index >= totalReaders) {  
            return null;  
        }  
        Thread thread = new LoggingThread(run,index);
        return new ThreadAndPosition(thread, threadStartingPositions[index]);
    }  
  
    @Override  
    public Thread writterThread(Runnable run) {
        int index = Integer.MAX_VALUE;
        synchronized (writerLock) {  
            if (writerThread == null) {  
                writerThread = new LoggingThread(run,index);
            }  
            return writerThread;  
        }  
    }  
  
    private static class LoggingThread extends Thread {
        private final int index;
        private final ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
        public LoggingThread(Runnable target, int index) {
            super(target);
            this.index = index;
        }
  
        @Override  
        public void run() {
            scheduler.scheduleAtFixedRate(() -> logState(String.format("Thread %d state",index)), 0, 5, TimeUnit.MILLISECONDS);
            try {
                super.run();
            } finally {
                logState(String.format("Thread %d finished", index));
                scheduler.shutdown();
            }
        }  
  
        private void logState(String message) {  
            logger.log(Level.INFO, "{0}: {1}", new Object[]{message, this.getState()});  
        }  
    }
}  