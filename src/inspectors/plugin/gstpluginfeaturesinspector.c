/*
 *  Plugin features inspector implementation
 */
#include "gstinspectors.h"
#include "gstinspector_priv.h"

/** 
 *  @addtogroup plugin-inspectors 
 *  @{ 
 */

/**
 *  @brief Structure collecting parsed features. Passed to a plugin's 
 *      feature lisr foreach function.
 */
typedef struct _InspectedPluginFeatures
{
    GstPlugin *plugin;          /**< Plugin to parse */
    GValue elements_array;      /**< Plugin to parse */
    GValue typefinds_array;     /**< Parsed typefinders */
    GValue tracers_array;       /**< Parsed tracers */
    GValue devproviders_array;  /**< Parsed device providers */
    GValue others_array;        /**< Other parsed features (other meaning non of the above) */
} InspectedPluginFeatures;

static void parse_typefind(GstPluginFeature *feature, InspectedPluginFeatures *features)
{
    GstTypeFindFactory *factory;
    const gchar *const *extensions;
    GString *typefind_str = g_string_new("");

    factory = GST_TYPE_FIND_FACTORY(feature);
    extensions = gst_type_find_factory_get_extensions(factory);

    g_string_printf(typefind_str, "%s: ", gst_plugin_feature_get_name(feature));
    if (extensions)
    {
        guint i = 0;

        while (extensions[i])
        {
            g_string_append_printf(typefind_str, "%s%s", i > 0 ? ", " : "", extensions[i]);
            i++;
        }
    }
    else
    {
        g_string_append(typefind_str, "no extensions");
    }

    if (G_UNLIKELY(!G_IS_VALUE(&features->typefinds_array)))
    {
        g_value_init(&features->typefinds_array, GST_TYPE_LIST);
    }

    gst_array_append_string(&features->typefinds_array, g_string_free(typefind_str, FALSE));
}

static void parse_feature(GstPluginFeature *feature, InspectedPluginFeatures *features)
{
    if (GST_IS_ELEMENT_FACTORY(feature))
    {
        GstElementFactory *factory = GST_ELEMENT_FACTORY(feature);

        if (G_UNLIKELY(!G_IS_VALUE(&features->elements_array)))
        {
            g_value_init(&features->elements_array, GST_TYPE_LIST);
        }
        gst_array_append_string(&features->elements_array,
                                g_strdup_printf("%s: %s", GST_OBJECT_NAME(factory),
                                                gst_element_factory_get_metadata(factory,
                                                                                 GST_ELEMENT_METADATA_LONGNAME)));
    }
    else if (GST_IS_TYPE_FIND_FACTORY(feature))
    {
        parse_typefind(feature, features);
    }
    else if (GST_IS_DEVICE_PROVIDER_FACTORY(feature))
    {
        GstDeviceProviderFactory *factory = GST_DEVICE_PROVIDER_FACTORY(feature);

        if (G_UNLIKELY(!G_IS_VALUE(&features->devproviders_array)))
        {
            g_value_init(&features->devproviders_array, GST_TYPE_LIST);
        }

        gst_array_append_string(&features->devproviders_array,
                                g_strdup_printf("%s: %s", GST_OBJECT_NAME(factory),
                                                gst_device_provider_factory_get_metadata(factory,
                                                                                         GST_ELEMENT_METADATA_LONGNAME)));
    }
    else if (GST_IS_TRACER_FACTORY(feature))
    {
        if (G_UNLIKELY(!G_IS_VALUE(&features->tracers_array)))
        {
            g_value_init(&features->tracers_array, GST_TYPE_LIST);
        }

        gst_array_append_string(&features->tracers_array,
                                g_strdup_printf("%s (%s)", GST_OBJECT_NAME(feature),
                                                g_type_name(gst_tracer_factory_get_tracer_type(
                                                    GST_TRACER_FACTORY_CAST(feature)))));
    }
    else if (feature)
    {
        if (G_UNLIKELY(!G_IS_VALUE(&features->others_array)))
        {
            g_value_init(&features->others_array, GST_TYPE_LIST);
        }

        gst_array_append_string(&features->others_array,
                                g_strdup_printf("%s (%s)", GST_OBJECT_NAME(feature),
                                                g_type_name(G_OBJECT_TYPE(feature))));
    }
}

/**
 *  @brief Inspects plugin features
 * 
 *  Inspects the features of a single plugin, as a list of the different types of features.
 * 
 *  @param plugin Plugin to inspect
 *  @param result 
 *  @parblock
 *  The inspected data
 * 
 *  The inspected data consists of these fields:
 *  - <b>Total features</b> - Total number of features in the plugin, as string (for convenience)
 *  - <b>Elements</b> - List of elements in the plugin, in the format of 
 *      <tt>[element_name]: [element_description]</tt>
 *  - <b>Typefinders</b> - List of typefinders in the plugin, in the format of 
 *      <tt>[typefinder_name]: [extensions]</tt>
 *  - <b>Device providers</b> - List of device providers in the plugin, in the format of 
 *      <tt>[device_provider_name]: [device_provider_description]</tt>
 *  - <b>Tracers</b> - List of tracers in the plugin, in the format of 
 *      <tt>[tracer_name] ([tracer_type])</tt>
 *  - <b>Other objects</b> - List of other feature objects in the plugin, in the format of 
 *      <tt>[object_name] ([object_type])</tt>
 *  @endparblock
 */
void gst_inspector_inspect_plugin_features(GstPlugin *plugin, GValue *result)
{
    GstStructure *dictionary;
    GList *features;
    InspectedPluginFeatures *inspected_features =
        (InspectedPluginFeatures *)g_new0(InspectedPluginFeatures, 1);
    inspected_features->plugin = plugin;

    features = gst_registry_get_feature_list_by_plugin(gst_registry_get(),
                                                       gst_plugin_get_name(plugin));

    dictionary = gst_structure_new_empty("pluginfeatures");
    g_value_init(result, GST_TYPE_STRUCTURE);

    g_list_foreach(features, (GFunc)parse_feature, inspected_features);

    gst_dictionary_set_string(dictionary, "Total features",
                              g_strdup_printf("%u", g_list_length(features)));

    gst_plugin_feature_list_free(features);

    if (G_IS_VALUE(&inspected_features->elements_array))
    {
        gst_dictionary_set_array(dictionary, "Elements", &inspected_features->elements_array);
    }
    if (G_IS_VALUE(&inspected_features->typefinds_array))
    {
        gst_dictionary_set_array(dictionary, "Typefinders", &inspected_features->typefinds_array);
    }
    if (G_IS_VALUE(&inspected_features->devproviders_array))
    {
        gst_dictionary_set_array(dictionary, "Device providers",
                                 &inspected_features->devproviders_array);
    }
    if (G_IS_VALUE(&inspected_features->tracers_array))
    {
        gst_dictionary_set_array(dictionary, "Tracers", &inspected_features->tracers_array);
    }
    if (G_IS_VALUE(&inspected_features->others_array))
    {
        gst_dictionary_set_array(dictionary, "Other objects", &inspected_features->others_array);
    }

    g_value_take_boxed(result, dictionary);
    g_free(inspected_features);
}

/** @}*/
