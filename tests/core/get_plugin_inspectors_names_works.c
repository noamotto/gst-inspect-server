#include "gstinspector_priv.h"

static gchar *test_list[] = {"test1", "test2", "test3"};

static GstStructure *fake_inspector(GstPlugin *plugin)
{
    (void)plugin;
    return NULL;
}

int main(int argc, char *argv[])
{
    gchar ** names = NULL;

    gst_init(&argc, &argv);

    gst_inspector_set_testing_mode();

    gst_inspector_register_plugin_inspector(fake_inspector, test_list[0], -1);
    gst_inspector_register_plugin_inspector(fake_inspector, test_list[1], -1);
    gst_inspector_register_plugin_inspector(fake_inspector, test_list[2], -1);

    names = gst_inspector_get_installed_plugin_inspectors();
    for (gsize i=0;i<3;i++)
    {
        g_assert_cmpstr(names[i], ==, test_list[i]);
    }

    g_free(names);

    return 0;
}