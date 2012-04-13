/*
 * epgfixer.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/plugin.h>
#include <vdr/i18n.h>
#include "regexp.h"
#include "setup_menu.h"
#include "epghandler.h"

#if defined(APIVERSNUM) && APIVERSNUM < 10726
#error "VDR-1.7.26 API version or greater is required!"
#endif

static const char *VERSION        = "0.0.6";
static const char *DESCRIPTION    = trNOOP("Fix bugs in EPG");

class cPluginEpgfixer : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginEpgfixer(void);
  virtual ~cPluginEpgfixer();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual time_t WakeupTime(void);
  virtual const char *MainMenuEntry(void);
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginEpgfixer::cPluginEpgfixer(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginEpgfixer::~cPluginEpgfixer()
{
  // Clean up after yourself!
}

const char *cPluginEpgfixer::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginEpgfixer::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginEpgfixer::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  EpgfixerRegexps.SetRegexpConfigFile(AddDirectory(cPlugin::ConfigDirectory(PLUGIN_NAME_I18N), "regexp.conf")); // allowed only via main thread!);
  EpgfixerRegexps.ReloadRegexps();
  return new cEpgfixerEpgHandler();
}

bool cPluginEpgfixer::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginEpgfixer::Stop(void)
{
  // Stop any background activities the plugin is performing.
}

void cPluginEpgfixer::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

const char *cPluginEpgfixer::MainMenuEntry(void)
{
  return NULL;
}

void cPluginEpgfixer::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginEpgfixer::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

time_t cPluginEpgfixer::WakeupTime(void)
{
  // Return custom wakeup time for shutdown script
  return 0;
}

cOsdObject *cPluginEpgfixer::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return NULL;
}

cMenuSetupPage *cPluginEpgfixer::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupEpgfixer();
}

bool cPluginEpgfixer::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return EpgfixerSetup.SetupParse(Name, Value);
}

bool cPluginEpgfixer::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginEpgfixer::SVDRPHelpPages(void)
{
  static const char *HelpPages[] = {
    "RLRE\n"
    "    Reload regexp.conf.",
    NULL
    };
  return HelpPages;
}

cString cPluginEpgfixer::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  if (strcasecmp(Command, "RLRE") == 0) {
     if (EpgfixerRegexps.ReloadRegexps()) {
        return cString("Reloaded regexp.conf");
     } else {
        ReplyCode = 554; // Requested action failed
        return cString("Reloading regexp.conf failed");
     }
  }
  return NULL;
}

VDRPLUGINCREATOR(cPluginEpgfixer); // Don't touch this!
