#include <alsa/asoundlib.h>

static char * card = "default";
static int mute_state = 2;

static void open_mixer(snd_mixer_t ** mixer)
{
  int err = snd_mixer_open(mixer, 0);
  if (err < 0) {
    fprintf(stderr, "Error: Cannot open mixer for card %s: %s\n",
            card, snd_strerror(err));
    exit(EXIT_FAILURE);
  }
}

static void attach_mixer(snd_mixer_t * mixer)
{
  int err = snd_mixer_attach(mixer, card);
  if (err < 0) {
    fprintf(stderr, "Cannot attach mixer interface to mixer device %s: %s\n",
            card, snd_strerror(err));
    snd_mixer_close(mixer);
    exit(EXIT_FAILURE);
  }
}

static void load_mixer(snd_mixer_t * mixer)
{
  int err = snd_mixer_load(mixer);
  if (err < 0) {
    fprintf(stderr, "Cannot load mixer: %s\n",
            snd_strerror(err));
    snd_mixer_close(mixer);
    exit(EXIT_FAILURE);
  }
}

static int callback(snd_mixer_t * mixer, unsigned mask, snd_mixer_elem_t *elem)
{
  return 0;
}

static void register_mixer_callback(snd_mixer_t * mixer)
{
  snd_mixer_set_callback(mixer, callback);
}

static void usage(const char * executable_name)
{
  printf("usage: %s 'on-mute-command' 'on-unmute-command'\n"
         "on-mute-command:   The command to execute when the sound is muted\n"
         "on-unmute-command: The command to execute when sound is restored\n",
         executable_name);
  exit(EXIT_FAILURE);
}

int check_mute_state_change(void)
{
  int new_mute_state = !system("/usr/bin/amixer get Master | /bin/grep -q off");
  if (new_mute_state != mute_state) {
    mute_state = new_mute_state;
    if (mute_state)
      return -1;
    else
      return +1;
  }
  return 0;
}

int main(int argc, char**argv)
{
  if (argc != 3) {
    usage(argv[0]);
  }
  const char * on_mute_command = argv[1];
  const char * on_unmute_command = argv[2];
  snd_mixer_t * mixer = NULL;
  open_mixer(&mixer);
  attach_mixer(mixer);
  load_mixer(mixer);
  register_mixer_callback(mixer);
  while (1) {
    int mute_state_change = check_mute_state_change();
    if (mute_state_change > 0)
      system(on_unmute_command);
    if (mute_state_change < 0)
      system(on_mute_command);

    int err = 0;
    err = snd_mixer_wait(mixer, -1);
    if (err >= 0) {
      err = snd_mixer_handle_events(mixer);
      if (err < 0) {
        fprintf(stderr, "mixer event handling caused error: %s\n",
                snd_strerror(err));
        return EXIT_FAILURE;
      }
    } else {
      return EXIT_FAILURE;
    }
  }
}

// Local Variables:
// indent-tabs-mode: nil
// c-basic-offset: 2
// coding: utf-8-unix
// End:
