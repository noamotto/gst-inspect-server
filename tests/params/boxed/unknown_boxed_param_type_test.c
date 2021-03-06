#include "type_reader/type_reader.h"
#include "gfakeboxed.h"

int main(int argc, char *argv[])
{
    GParamSpec *boxed_spec = NULL;
    const GValue *value = NULL;
    GstStructure *dictionary;
    gchar *type_string = g_strdup_printf("Boxed pointer of type \"%s\"",
                                         g_type_name(G_TYPE_FAKE_BOXED));

    gst_init(&argc, &argv);

    dictionary = gst_structure_new_empty("test");
    boxed_spec = g_param_spec_boxed("test", "Test param",
                                    "Test param", G_TYPE_FAKE_BOXED, G_PARAM_READWRITE);
    value = g_param_spec_get_default_value(boxed_spec);

    gst_type_reader_fill_type(boxed_spec, value, dictionary);

    g_assert_true(gst_structure_has_field_typed(dictionary, KEY_NAME, G_TYPE_STRING));
    g_assert_cmpstr(gst_dictionary_get_string(dictionary, KEY_NAME), ==,"test: Test param");

    g_assert_true(gst_structure_has_field_typed(dictionary, KEY_TYPE, G_TYPE_STRING));
    g_assert_cmpstr(gst_dictionary_get_string(dictionary, KEY_TYPE),==, type_string);

    g_assert_false(gst_structure_has_field(dictionary, KEY_VALUE));

    g_free(type_string);
    g_param_spec_unref(boxed_spec);
    gst_structure_free(dictionary);
    gst_deinit();

    return 0;
}
