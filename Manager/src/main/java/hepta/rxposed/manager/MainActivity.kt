/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package hepta.rxposed.manager;

import android.content.res.Resources
import android.os.Bundle
import android.view.Menu
import android.view.MenuInflater
import android.view.MenuItem
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.widget.Toolbar
import androidx.core.view.MenuProvider
import androidx.drawerlayout.widget.DrawerLayout
import androidx.lifecycle.Lifecycle
import androidx.navigation.NavController
import androidx.navigation.findNavController
import androidx.navigation.fragment.NavHostFragment
import androidx.navigation.ui.*
import com.google.android.material.navigation.NavigationView
import hepta.rxposed.manager.fragment.LoadExten.ExtenInfoProvider
import hepta.rxposed.manager.fragment.PlugInject.SupportInfoProvider
import hepta.rxposed.manager.util.InjectTool

/**
 * A simple activity demonstrating use of a NavHostFragment with a navigation drawer.
 */
class MainActivity : AppCompatActivity() {
    private lateinit var toolbar: Toolbar
    private var toolbarHeight: Int = 0
    private lateinit var appBarConfiguration : AppBarConfiguration


//    private fun initConfig() {
//        object : Thread() {
//            override fun run() {
//                SupportInfoProvider.getInstance().init()
//                ExtenInfoProvider.getInstance().init()
//                InjectTool.init()
//            }
//        }.start()
//    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

//        initConfig();
        setContentView(R.layout.activity_navigation)

        toolbar = findViewById<Toolbar>(R.id.toolbar)
        toolbarHeight= toolbar.layoutParams.height
        setSupportActionBar(toolbar)

        val host: NavHostFragment = supportFragmentManager.findFragmentById(R.id.my_nav_host_fragment) as NavHostFragment? ?: return

        // Set up Action Bar
        val navController = host.navController

//        appBarConfiguration = AppBarConfiguration(navController.graph)

        val drawerLayout : DrawerLayout? = findViewById(R.id.drawer_layout)
        appBarConfiguration = AppBarConfiguration(setOf(R.id.home_dest, R.id.processlist_dest, R.id.check_dest), drawerLayout)

        setupActionBar(navController, appBarConfiguration)

        setupNavigationMenu(navController)

//        setupBottomNavMenu(navController)

        navController.addOnDestinationChangedListener { _, destination, _ ->
            val dest: String = try {
                resources.getResourceName(destination.id)
            } catch (e: Resources.NotFoundException) {
                Integer.toString(destination.id)
            }
        }
        addMenuProvider(object :MenuProvider{
            override fun onCreateMenu(menu: Menu, menuInflater: MenuInflater) {
            }

            override fun onMenuItemSelected(menuItem: MenuItem): Boolean {
                return menuItem.onNavDestinationSelected(findNavController(R.id.my_nav_host_fragment))
            }

        },this, Lifecycle.State.RESUMED)

    }


    private fun setupNavigationMenu(navController: NavController) {

        val sideNavView = findViewById<NavigationView>(R.id.nav_view)
        sideNavView?.setupWithNavController(navController)
    }

    private fun setupActionBar(navController: NavController,
                               appBarConfig : AppBarConfiguration) {

        setupActionBarWithNavController(navController, appBarConfig)
    }


    override fun onSupportNavigateUp(): Boolean {
        return findNavController(R.id.my_nav_host_fragment).navigateUp(appBarConfiguration)
    }

    public  fun DisableToolBar() {
        toolbar.layoutParams.height = 0

    }

    public  fun EnableToolBar() {

        toolbar.layoutParams.height = toolbarHeight

    }
}
