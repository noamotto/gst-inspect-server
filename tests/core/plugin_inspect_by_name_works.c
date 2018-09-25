#include "gstinspector.h"
#include "gsttestplugin.h"

#define PLUGIN_NAME         ("testplugin")
#define TEST_NAME           ("test")
#define TEST_FIELD_NAME     ("Test field")
#define TEST_FIELD_VALUE    ("hello")

static GstStructure *fake_inspector(GstPlugin *plugin)
{
    (void)plugin;
    GstStructure *test = gst_structure_new_empty(TEST_NAME);
    gst_dictionary_set_string(test, TEST_FIELD_NAME, TEST_FIELD_VALUE);
    return test;
}

int main(int argc, char *argv[])
{
    GstStructure *data;
    GstStructure *inspector_data;
    const gchar *test_field_value;

    gst_init(&argc, &argv);
    GST_PLUGIN_STATIC_REGISTER(testplugin);

    gst_inspector_register_plugin_inspector(fake_inspector, TEST_NAME, -1);

    data = gst_inspector_inspect_by_name(PLUGIN_NAME);

    g_assert_cmpstr(PLUGIN_NAME, ==, gst_structure_get_name(data));

    inspector_data = gst_dictionary_get_sub_dictionary(data, TEST_NAME);
    g_assert_nonnull(inspector_data);

    test_field_value = gst_dictionary_get_string(inspector_data, TEST_FIELD_NAME);
    g_assert_cmpstr(test_field_value, ==, TEST_FIELD_VALUE);

    return 0;
}