// IRxposedService.aidl
package hepta.rxposed.manager;

// Declare any non-default types here with import statements

interface IRxposedService {
    /**
     * Demonstrates some basic types that you can use as parameters
     * and return values in AIDL.
     */
    void basicTypes(int anInt, long aLong, boolean aBoolean, float aFloat,
            double aDouble, String aString);

    String getConfig(String Name);

}