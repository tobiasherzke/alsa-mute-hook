#include <alsa/asoundlib.h>

static char * card = "default";

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
  printf("mixer callback called with mask 0x%x\n", mask);
  return 0;
}

static void register_mixer_callback(snd_mixer_t * mixer)
{
  snd_mixer_set_callback(mixer, callback);
}

int main(int argc, char**argv)
{
  snd_mixer_t * mixer = NULL;
  open_mixer(&mixer);
  attach_mixer(mixer);
  load_mixer(mixer);
  register_mixer_callback(mixer);
  while (1) {
    int err = 0;
    printf("Calling snd_mixer_wait\n");
    err = snd_mixer_wait(mixer, -1);
    if (err >= 0) {
      printf("mixer_wait returned successfully.\n");
      err = snd_mixer_handle_events(mixer);
      if (err < 0) {
	fprintf(stderr, "mixer event handling caused error: %s\n",
		snd_strerror(err));
	return EXIT_FAILURE;
      }
    } else {
      fprintf(stderr, "mixer_wait caused error: %s\n", snd_strerror(err));
      return EXIT_FAILURE;
    }
  }
}
