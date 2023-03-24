package hepta.rxposed.loadxposed

import android.content.res.Resources
import android.os.Bundle
import com.google.android.material.bottomnavigation.BottomNavigationView
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.widget.Toolbar
import androidx.drawerlayout.widget.DrawerLayout
import androidx.navigation.NavController
import androidx.navigation.findNavController
import androidx.navigation.fragment.NavHostFragment
import androidx.navigation.ui.AppBarConfiguration
import androidx.navigation.ui.navigateUp
import androidx.navigation.ui.setupActionBarWithNavController
import androidx.navigation.ui.setupWithNavController
import com.google.android.material.navigation.NavigationView
import hepta.rxposed.loadxposed.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    private lateinit var toolbar: Toolbar
    private var toolbarHeight: Int = 0
    private lateinit var appBarConfiguration: AppBarConfiguration

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_navigation)

        toolbar = findViewById<Toolbar>(R.id.toolbar)
        toolbarHeight = toolbar.layoutParams.height
        setSupportActionBar(toolbar)

        val host: NavHostFragment =
            supportFragmentManager.findFragmentById(R.id.my_nav_host_fragment) as NavHostFragment?
                ?: return

        // Set up Action Bar
        val navController = host.navController

//        appBarConfiguration = AppBarConfiguration(navController.graph)

        // TODO STEP 9.5 - Create an AppBarConfiguration with the correct top-level destinations
        // You should also remove the old appBarConfiguration setup above
        val drawerLayout: DrawerLayout? = findViewById(R.id.drawer_layout)
        appBarConfiguration = AppBarConfiguration(
            setOf(R.id.navigation_home, R.id.navigation_dashboard, R.id.navigation_notifications),
            drawerLayout
        )
        // TODO END STEP 9.5

        setupActionBar(navController, appBarConfiguration)

        setupNavigationMenu(navController)

//        setupBottomNavMenu(navController)

        navController.addOnDestinationChangedListener { _, destination, _ ->
            val dest: String = try {
                resources.getResourceName(destination.id)
            } catch (e: Resources.NotFoundException) {
                Integer.toString(destination.id)
            }

//            Toast.makeText(this@MainActivity, "Navigated to $dest",
//                    Toast.LENGTH_SHORT).show()
//            Log.d("NavigationActivity", "Navigated to $dest")
        }
    }
    private fun setupActionBar(navController: NavController,
                               appBarConfig : AppBarConfiguration) {

        setupActionBarWithNavController(navController, appBarConfig)
        // TODO END STEP 9.6
    }

    private fun setupNavigationMenu(navController: NavController) {

        val sideNavView = findViewById<NavigationView>(R.id.nav_view)
        sideNavView?.setupWithNavController(navController)
    }

    override fun onSupportNavigateUp(): Boolean {

        return findNavController(R.id.my_nav_host_fragment).navigateUp(appBarConfiguration)
    }

    public  fun DisableToolBar() {
        toolbar.layoutParams.height = 0

    }

    public  fun EnableToolBar() {

        toolbar.layoutParams.height = toolbarHeight
        // Allows NavigationUI to support proper up navigation or the drawer layout
        // drawer menu, depending on the situation
    }

}