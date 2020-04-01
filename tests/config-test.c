#include <hyscan-config.h>

int
main (void)
{
  const gchar ** pfd = hyscan_config_get_profile_dirs ();
  const gchar * lcd = hyscan_config_get_locale_dir ();
  const gchar * ufd = hyscan_config_get_user_files_dir ();

  for (; pfd != NULL && *pfd != NULL; )
    {
      g_message("profile directory: <%s>", *pfd);
      pfd++;
    }
  g_message("locale directory: <%s>", lcd);
  g_message("user files directory: <%s>", ufd);
}
