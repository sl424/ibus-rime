#include "rime_config.h"
#include "rime_settings.h"
#include <rime_api.h>
#include <string.h>

extern RimeApi *rime_api;

static struct ColorSchemeDefinition preset_color_schemes[] = {
  { "aqua", 0xffffff, 0x0a3dfa },
  { "azure", 0xffffff, 0x0a3dea },
  { "ink", 0xffffff, 0x000000 },
  { "luna", 0x000000, 0xffff7f },
  { NULL, 0, 0 }
};

static struct IBusRimeSettings ibus_rime_settings_default = {
  TRUE,
  IBUS_ORIENTATION_SYSTEM,
  &preset_color_schemes[2],
};

struct IBusRimeSettings g_ibus_rime_settings;

static void
select_color_scheme(struct IBusRimeSettings* settings,
		    const char* color_scheme_id)
{
  struct ColorSchemeDefinition* c;
  for (c = preset_color_schemes; c->color_scheme_id; ++c) {
    if (!strcmp(c->color_scheme_id, color_scheme_id)) {
      settings->color_scheme = c;
      g_debug("selected color scheme: %s", color_scheme_id);
      return;
    }
  }
  // fallback to default
  settings->color_scheme = &preset_color_schemes[2];
}

void
ibus_rime_load_settings()
{
  g_ibus_rime_settings = ibus_rime_settings_default;

  RimeConfig config = {0};
  if (!rime_api->config_open("ibus_rime", &config)) {
    g_error("error loading settings for ibus_rime");
    return;
  }

  Bool inline_preedit = False;
  if (rime_api->config_get_bool(
          &config, "style/inline_preedit", &inline_preedit)) {
    g_ibus_rime_settings.embed_preedit_text = !!inline_preedit;
  }

  Bool horizontal = False;
  if (rime_api->config_get_bool(&config, "style/horizontal", &horizontal)) {
    g_ibus_rime_settings.lookup_table_orientation =
      horizontal ? IBUS_ORIENTATION_HORIZONTAL : IBUS_ORIENTATION_VERTICAL;
  }

  const char* color_scheme =
    rime_api->config_get_cstring(&config, "style/color_scheme");
  if (color_scheme) {
    select_color_scheme(&g_ibus_rime_settings, color_scheme);
  }

  rime_api->config_close(&config);
}
