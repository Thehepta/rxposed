package hepta.rxposed.rxposedtool

class NativeLib {

    /**
     * A native method that is implemented by the 'rxposedtool' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'rxposedtool' library on application startup.
        init {
            System.loadLibrary("rxposedtool")
        }
    }
}