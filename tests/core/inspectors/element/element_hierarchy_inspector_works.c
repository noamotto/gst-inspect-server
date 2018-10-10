#include "gstinspectors.h"
#include "testutils.h"

#define FACTORY_NAME ("testelement")

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

void gst_test_element_class_init(GstTestElementClass *klass)
{
    gst_element_class_set_static_metadata(GST_ELEMENT_CLASS(klass),
                                          "testelement",
                                          "TEST",
                                          "Test Element",
                                          "Noam Ottolenghi");
}

void gst_test_element_init(GstTestElement *self)
{
    (void)self;
}

int main(int argc, char *argv[])
{
    GstElement *element;
    GValue result = G_VALUE_INIT;
    GValue hierarchy_arr = G_VALUE_INIT;
 
    gst_init(&argc, &argv);
    gst_element_register(NULL, FACTORY_NAME, GST_RANK_NONE, GST_TYPE_TEST_ELEMENT);


    element = gst_element_factory_make(FACTORY_NAME, NULL);
    gst_inspector_inspect_element_hierarchy(element, &result);

    g_assert_true(GST_VALUE_HOLDS_ARRAY(&result));
    g_value_init(&hierarchy_arr, GST_TYPE_ARRAY);
    gst_array_append_static_string(&hierarchy_arr, "GObject");
    gst_array_append_static_string(&hierarchy_arr, "GInitiallyUnowned");
    gst_array_append_static_string(&hierarchy_arr, "GstObject");
    gst_array_append_static_string(&hierarchy_arr, "GstElement");
    gst_array_append_static_string(&hierarchy_arr, "GstTestElement");

    g_assert_true(gst_value_compare(&hierarchy_arr, &result) == GST_VALUE_EQUAL);

    g_value_unset(&hierarchy_arr);
    gst_object_unref(element);

    return 0;
}
