package mr.demonid;

public class Main {

    public native int logOpenConsole();
    public native void logClose();
    public native void logPrintf(String format, Object ... args);


    public static void main(String[] args) throws InterruptedException {

        System.loadLibrary("mcon");
        System.out.println("start...");
        Main main = new Main();

        if (main.logOpenConsole() != 0)
        {
            System.out.println("do printf...");
            main.logPrintf("Test printf '%s', int: %d, float: %f, long: %l, double: %f\n", "str", 12, 13.23f, 1223L, 927.23311);
        }

        System.out.println("Hello world!");
        Thread.sleep(15000);
        main.logClose();
    }
}