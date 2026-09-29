#include <elog.h>
#include <unistd.h>
#include <sys/types.h>
#include <oui.h>

void test_vendor_c()
{
	oui_handler_t *oui = load_available_oui();
	if (!oui) {
		elog_printf(ELOG_ERR, "load_available_oui failed\n");
		return;
	}

	int vid;
	vid = get_vendor_id(oui, "AA:BB:CC:DD:EE:FF");
	elog_printf(ELOG_INFO, "MAC AA:BB:CC, VID %d\n", vid);

	vid = get_vendor_id(oui, "7C:D1:C3:00:12:E9");
	elog_printf(ELOG_INFO, "MAC 7C:D1:C3:00:12:E9, VID %d\n", vid);
    release_oui(oui);
}
int main(int argc, char *argv[])
{
	if (argc > 1)
		elog_init(ELOGBACK_DISKFILE, argv[1]);
	else
		elog_init(0);

	elog_printf(ELOG_WARNING, "This is uhmi test program[%d]\n", (int)getpid());

	elog_printf(ELOG_INFO, "Now test OUI functions --\n");
	test_vendor_c();
	return 0;
}
