package hepta.rxposed.manager.util

import com.google.gson.Gson
import com.tencent.mmkv.MMKV
import java.util.*

object MmkvManager {

    const val ID_MAIN = "MAIN"
    const val ID_SERVER_CONFIG = "SERVER_CONFIG"
    const val ID_SERVER_RAW = "SERVER_RAW"
    const val ID_SERVER_AFF = "SERVER_AFF"
    const val ID_SETTING = "SETTING"
    const val KEY_SELECTED_SERVER = "SELECTED_SERVER"
    const val KEY_ANG_CONFIGS = "ANG_CONFIGS"

    const val KEY_APP_ALLWO_NONE = 0
    const val KEY_APP_ADD_ALLOW = 1
    const val KEY_APP_ADD_ALLOW_LIST = "KEY_APP_ADD_ALLOW_LIST"

    const val KEY_APP_ADD_DIS_ALLOW = 2
    const val KEY_APP_ADD_DIS_ALLOW_LIST = "KEY_APP_ADD_DIS_ALLOW_LIST"


    private val mainStorage by lazy { MMKV.mmkvWithID(ID_MAIN, MMKV.MULTI_PROCESS_MODE) }
    private val serverStorage by lazy { MMKV.mmkvWithID(ID_SERVER_CONFIG, MMKV.MULTI_PROCESS_MODE) }
    private val serverAffStorage by lazy { MMKV.mmkvWithID(ID_SERVER_AFF, MMKV.MULTI_PROCESS_MODE) }
    private val setStorage by lazy { MMKV.mmkvWithID(ID_SETTING, MMKV.MULTI_PROCESS_MODE) }


    fun decodeServerList(): MutableList<String> {
        val json = mainStorage?.decodeString(KEY_ANG_CONFIGS)
        return if (json.isNullOrBlank()) {
            mutableListOf()
        } else {
            Gson().fromJson(json, Array<String>::class.java).toMutableList()
        }
    }

    fun decodeServerConfig(guid: String): ServerConfig? {
        if (guid.isBlank()) {
            return null
        }
        val json = serverStorage?.decodeString(guid)
        if (json.isNullOrBlank()) {
            return null
        }
        return Gson().fromJson(json, ServerConfig::class.java)
    }

    fun encodeServerConfig(guid: String, config: ServerConfig): String {
        val key = guid.ifBlank { getUuid() }
        serverStorage?.encode(key, Gson().toJson(config))
        val serverList = decodeServerList()
        if (!serverList.contains(key)) {
            serverList.add(0, key)
            mainStorage?.encode(KEY_ANG_CONFIGS, Gson().toJson(serverList))
            if (mainStorage?.decodeString(KEY_SELECTED_SERVER).isNullOrBlank()) {
                mainStorage?.encode(KEY_SELECTED_SERVER, key)
            }
        }
        return key
    }
    private fun getUuid() :String{
        return try {
            UUID.randomUUID().toString().replace("-", "")
        } catch (e: Exception) {
            e.printStackTrace()
            ""
        }
    }

    fun removeServer(guid: String) {
        if (guid.isBlank()) {
            return
        }
        if (mainStorage?.decodeString(KEY_SELECTED_SERVER) == guid) {
            mainStorage?.remove(KEY_SELECTED_SERVER)
        }
        val serverList = decodeServerList()
        serverList.remove(guid)
        mainStorage?.encode(KEY_ANG_CONFIGS, Gson().toJson(serverList))
        serverStorage?.remove(guid)
        serverAffStorage?.remove(guid)
    }



    fun decodeApplicationList(type: Int): MutableList<String> {
        val json = if (type==KEY_APP_ADD_ALLOW){
            setStorage?.decodeString(KEY_APP_ADD_ALLOW_LIST)
        }else{
            setStorage?.decodeString(KEY_APP_ADD_DIS_ALLOW_LIST)
        }
        return if (json.isNullOrBlank()) {
            mutableListOf()
        } else {
            Gson().fromJson(json, Array<String>::class.java).toMutableList()
        }
    }

    fun encodeApplicationList( list :MutableList<String>,type: Int) {
        val json = Gson().toJson(list)
        if (type==KEY_APP_ADD_ALLOW){
            setStorage?.encode(KEY_APP_ADD_ALLOW_LIST,json)
        }else{
            setStorage?.encode(KEY_APP_ADD_DIS_ALLOW_LIST,json)
        }
    }

    fun getAllowType() :Int {
        return setStorage.decodeInt("Type",0)
    }

    fun setAllowType(type:Int) {
        setStorage.encode("Type", type)
    }


    fun getAddress():String{
        return setStorage.decodeString("address","10.0.0.2")!!

    }
    fun setAddress(address:String) {
        setStorage.encode("address",address)
    }

    fun getnetMask():String{
        return setStorage.decodeString("netmask","255.255.255.252")!!

    }
    fun setnetMask(address:String) {
        setStorage.encode("netmask",address)
    }

    fun getMtu():Int{
        return setStorage.decodeInt("mtu",1400)!!

    }
    fun setMtu(Mtu:Int) {
        setStorage.encode("mtu",Mtu)
    }
}