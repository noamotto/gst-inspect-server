#include "type_reader/type_reader_priv.h"

void gst_pointer_type_reader_fill_type(
    GParamSpec *const pspec,
    GValue *const value,
    GstStructure *const dictionary)
{
    GValue key_value = G_VALUE_INIT;

    (void)value;
    g_value_init(&key_value, G_TYPE_STRING);

    if (pspec->value_type != G_TYPE_POINTER)
    {
        g_value_take_string(&key_value, g_strdup_printf("Pointer of type \"%s\".",
                                                        g_type_name(pspec->value_type)));
        gst_structure_take_value(dictionary, KEY_TYPE, &key_value);
    }
    else
    {
        g_value_take_string(&key_value, g_strdup_printf("Pointer."));
        gst_structure_take_value(dictionary, KEY_TYPE, &key_value);
    }
}
