package hepta.rxposed.manager.util;

import android.content.Context;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import hepta.rxposed.manager.RxposedApp;

public class LogFileHelper {


    private static final String TAG = "LogFileHelper";

    public static void writeLog(String log) {
        File logFile = new File(Consts.LOG_FILE_NAME);
        try (FileOutputStream outputStream = new FileOutputStream(logFile, true)) {
            outputStream.write((log + "\n").getBytes());
        } catch (IOException e) {
            Log.e(TAG, "Error writing log to file", e);
        }
    }

    public static String readLog() {
        File logFile = new File(Consts.LOG_FILE_NAME);
        if (logFile.exists()) {
            try {
                return new String(java.nio.file.Files.readAllBytes(logFile.toPath()));
            } catch (IOException e) {
                Log.e(TAG, "Error reading log file", e);
                return "Error reading log file";
            }
        } else {
            return "No logs available";
        }
    }

    public static void clearLog() {
        File logFile = new File(Consts.LOG_FILE_NAME);
        try (FileOutputStream outputStream = new FileOutputStream(logFile)) {
            // Writing an empty string to the file will clear its content
            outputStream.write(new byte[0]);
        } catch (IOException e) {
            Log.e(TAG, "Error clearing log file", e);
        }
    }
}
