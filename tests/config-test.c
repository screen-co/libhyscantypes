#include <hyscan-config.h>

int
main (void)
{
  const gchar ** pfd = hyscan_config_get_profile_dirs ();
  const gchar * lcl = hyscan_config_get_locale_dir ();

  for (; pfd != NULL && *pfd != NULL; )
    {
      g_message("profile directory: <%s>", *pfd);
      pfd++;
    }
  g_message("locale directory: <%s>", lcl);
}
