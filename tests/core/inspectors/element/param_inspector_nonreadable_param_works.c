#include "gstinspectors.h"

#define FACTORY_NAME ("testelement")
#define TEST_PROP_ID (1)

typedef struct _GstTestElement
{
    GstElement element;
} GstTestElement;

typedef struct _GstTestElementClass
{
    GstElementClass klass;
} GstTestElementClass;

#define GST_TYPE_TEST_ELEMENT gst_test_element_get_type()
GType gst_test_element_get_type(void);

G_DEFINE_TYPE(GstTestElement, gst_test_element, GST_TYPE_ELEMENT)

void test_get_property(GObject *object,
                       guint property_id,
                       GValue *value,
                       GParamSpec *pspec);

void test_set_property(GObject *object,
                       guint property_id,
                       const GValue *value,
                       GParamSpec *pspec);

void gst_test_element_class_init(GstTestElementClass *klass)
{
    gst_element_class_set_static_metadata(GST_ELEMENT_CLASS(klass),
                                          "testelement",
                                          "TEST",
                                          "Test Element",
                                          "Noam Ottolenghi");

    G_OBJECT_CLASS(klass)->get_property = test_get_property;
    G_OBJECT_CLASS(klass)->set_property = test_set_property;
    g_object_class_install_property(G_OBJECT_CLASS(klass), TEST_PROP_ID,
                                    g_param_spec_boolean("param", "param",
                                                         "param", TRUE, G_PARAM_WRITABLE));
}

void gst_test_element_init(GstTestElement *self)
{
    (void)self;
}

void test_get_property(GObject *object,
                       guint property_id,
                       GValue *value,
                       GParamSpec *pspec)
{
    (void)value;
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
}

void test_set_property(GObject *object,
                       guint property_id,
                       const GValue *value,
                       GParamSpec *pspec)
{
    (void)value;
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
}

int main(int argc, char *argv[])
{
    GstElement *element;
    GValue result = G_VALUE_INIT;
    GstStructure *data;
    GstStructure *param_data;

    gst_init(&argc, &argv);

    gst_element_register(NULL, FACTORY_NAME, GST_RANK_NONE, GST_TYPE_TEST_ELEMENT);

    element = gst_element_factory_make(FACTORY_NAME, NULL);

    gst_inspector_inspect_element_properties(element, &result);

    g_assert_true(GST_VALUE_HOLDS_STRUCTURE(&result));

    data = g_value_get_boxed(&result);

    param_data = gst_dictionary_get_sub_dictionary(data, "param");
    g_assert_nonnull(param_data);

    g_assert_true(gst_structure_has_field_typed(param_data, KEY_NAME, G_TYPE_STRING));
    g_assert_cmpstr(gst_dictionary_get_string(param_data, KEY_NAME), ==, "param: param");

    g_assert_true(gst_structure_has_field_typed(param_data, KEY_TYPE, G_TYPE_STRING));
    g_assert_cmpstr(gst_dictionary_get_string(param_data, KEY_TYPE), ==, "Boolean");

    g_assert_true(gst_structure_has_field_typed(param_data, KEY_VALUE, G_TYPE_STRING));
    g_assert_cmpstr(gst_dictionary_get_string(param_data, KEY_VALUE), ==, "true");

    g_value_unset(&result);
    gst_object_unref(element);
    gst_deinit();

    return 0;
}
