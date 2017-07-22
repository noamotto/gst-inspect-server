#include "type_reader/type_reader.h"

#define MINIMUM (G_GINT64_CONSTANT(0))
#define MAXIMUM (G_MAXUINT64)
#define VALUE ((G_MAXUINT64 - 1) / 2)

int main(int argc, char *argv[])
{
    GParamSpec *uint64_spec = NULL;
    const GValue *value = NULL;
    GstStructure *dictionary = NULL;
    gchar *range_string = NULL;
    gchar *value_string = NULL;

    gst_init(&argc, &argv);

    uint64_spec = g_param_spec_uint64("test", "Test param",
                                "Test param", MINIMUM, MAXIMUM, VALUE, G_PARAM_READWRITE);
    value = g_param_spec_get_default_value(uint64_spec);
    range_string = g_strdup_printf("%" G_GUINT64_FORMAT " - %" G_GUINT64_FORMAT, MINIMUM, MAXIMUM);
    value_string = g_strdup_printf("%" G_GUINT64_FORMAT, VALUE);

    dictionary = gst_type_reader_fill_type(uint64_spec, value);

    g_assert_true(gst_structure_has_field_typed(dictionary, KEY_NAME, G_TYPE_STRING));
    g_assert_cmpstr(gst_structure_get_string(dictionary, KEY_NAME), ==, "test: Test param");

    g_assert_true(gst_structure_has_field_typed(dictionary, KEY_TYPE, G_TYPE_STRING));
    g_assert_cmpstr(gst_structure_get_string(dictionary, KEY_TYPE), ==, "Unsigned Integer64");

    g_assert_true(gst_structure_has_field_typed(dictionary, KEY_RANGE, G_TYPE_STRING));
    g_assert_cmpstr(gst_structure_get_string(dictionary, KEY_RANGE), ==, range_string);

    g_assert_true(gst_structure_has_field_typed(dictionary, KEY_VALUE, G_TYPE_STRING));
    g_assert_cmpstr(gst_structure_get_string(dictionary, KEY_VALUE), ==, value_string);

    g_free(range_string);
    g_free(value_string);
    return 0;
}