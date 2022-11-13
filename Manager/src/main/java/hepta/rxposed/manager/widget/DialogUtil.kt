package hepta.rxposed.manager.widget;
import android.content.Context
import com.afollestad.materialdialogs.LayoutMode
import com.afollestad.materialdialogs.MaterialDialog
import com.afollestad.materialdialogs.bottomsheets.BottomSheet
import hepta.rxposed.manager.R

class DialogUtil {

    companion object {

        fun DidalogSimple(context: Context, resId: Int, positcallback: () -> Unit, Tips: Int = R.string.appTips, Ok: Int = android.R.string.ok) {

            val reboot_dialog = MaterialDialog(context, BottomSheet(LayoutMode.WRAP_CONTENT)).show {
                title(Tips)
                message(resId)
                positiveButton(Ok)
                negativeButton(android.R.string.cancel)
            }
            reboot_dialog.positiveButton {
                positcallback()
            }
            reboot_dialog.negativeButton {
            }
        }
        fun DidalogSimple(context: Context, resId: String, positcallback: () -> Unit,Tips: Int = R.string.appTips,Ok: Int = android.R.string.ok) {

            val reboot_dialog = MaterialDialog(context, BottomSheet(LayoutMode.WRAP_CONTENT)).show {
                title(Tips)
                message(text = resId)
                positiveButton(Ok)
                negativeButton(android.R.string.cancel)
            }
            reboot_dialog.positiveButton {
                positcallback()
            }
            reboot_dialog.negativeButton {
            }
        }

    }
}