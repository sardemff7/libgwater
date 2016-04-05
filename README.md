libgwater
=========

libgwater provides several GSources to integrate various events to the GLib main loop.


Composition of a GSource
------------------------

Each GSource is composed of three parts:

-   An m4 macro for Autoconf: `GW_CHECK_<SOURCE_NAME>`
    It takes two optional arguments:

    1. Additionnal `pkg-config` packages to check
    2. Additionnal headers to check

-   A `Makefile.am` snippet for Automake: `libgwater/<source-name>.mk`

    You have to include it in your `Makefile.am`. It defines two variables:

    - `GW_<SOURCE_NAME>_CFLAGS`: contains all the needed `CFLAGS` (including those from the `pkg-config` packages you passed to the m4 macro)
    - `GW_<SOURCE_NAME>_LIBS`: contains all the needed `LIBS` (including those from the `pkg-config` packages you passed to the m4 macro)

    This snippet is using Automake 1.14 features and Libtool. If you do not use both in your project, you need to adapt and ship it in your project.

-   A really basic API:
    - A `GWater<SourceName>Source` opaque struct
    - One or more `g_water_<source_name>_source_new()` constructor for said struct
    - One free function `g_water_<source_name>_source_free()`
    - Usually a getter, to access the underlaying struct used by the communication library

Non-Autotools setup are not covered upstream but should be easy enough to use.


Example usage of XCB GSource
----------------------------

1. Create your project with Autotools files

2. Add libgwater as a Git submodule
    ```shell
    git submodule add git://github.com/sardemff7/libgwater
    ```

3. Add the m4 macro call to your `configure.ac` (with an additionnal package, `xcb-util`)
    ```m4
    GW_CHECK_XCB([xcb-util])
    ```

4. Make sure aclocal will find the m4 file by adding the directory to `ACLOCAL_AMFLAGS` in your `Makefile.am`
    ```Makefile
    ACLOCAL_AMFLAGS = -I libgwater ${ACLOCAL_FLAGS}
    ```

5. Include the `Makefile.am` snippet
    ```Makefile
    include $(top_srcdir)/libgwater/xcb.mk
    ```

6. Use the `Makefile.am` variables
    ```Makefile
    xcb_example_client_SOURCES = client.c
    xcb_example_client_CFLAGS = $(GW_XCB_CFLAGS)
    xcb_example_client_LDADD = $(GW_XCB_LIBS)
    ```

7. Use the GSource in your `client.c`
    ```c
    #include <glib.h>
    #include <libgwater-xcb.h>

    gboolean
    callback(xcb_generic_event_t *event, gpointer user_data)
    {
        if ( event == NULL )
        {
            /* Error occured */
            return FALSE;
        }
        return TRUE;
    }

    int
    main()
    {
        GWaterXcbSource *source;
        GMainLoop *loop;

        loop = g_main_loop_new(NULL, FALSE);
        source = g_water_xcb_source_new(NULL, NULL, NULL, callback, NULL, NULL);

        g_main_loop_run(loop);
        g_main_loop_unref(loop);

        g_xcb_source_free(source);

        return 0;
    }
    ```


Dependencies
------------

-   Common dependencies
    - GLib 2.36 (or newer)
-   Wayland source
    - libwayland 1.1.91 (or newer)
-   XCB source
    - libxcb
-   MPD source
    - libmpdclient2
-   ALSA Mixer source
    - alsa-lib
-   Netlink protocol source
    - libnl-3.0
-   Windows source
    - `Windows.h`


Licence
-------

libgwater is licenced under the terms of the [MIT license](//opensource.org/licenses/MIT)


Author / Contact
----------------

Quentin “Sardem FF7” Glidic (sardemff7+libgwater@sardemff7.net) — [My other Free Software projects](//www.sardemff7.net/)
