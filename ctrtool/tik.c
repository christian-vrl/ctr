#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tik.h"
#include "ctr.h"
#include "utils.h"

void tik_decrypt_titlekey(eticket *tik, u8 *out_buf) {
	u8 buf[16], key[16], iv[16];
	ctr_crypto_context ctx;

	memcpy(buf, tik->encrypted_title_key, 0x10);

	if (key_load("common-key", key) == -1) {
		exit(-1);
	}

	memset(iv, 0, 0x10);
	memcpy(iv, &tik->title_id, 8);

	ctr_init_cbc_decrypt(&ctx, key, iv);
	ctr_decrypt_cbc(&ctx, buf, out_buf, 0x10);
}

void tik_print(u8 *blob, u32 size) {
	int i;
	u8 dtk[0x10];


	eticket *tik = (eticket*)blob;

	fprintf(stdout,
		"Signature Type:         %08x\n"
		"Issuer:                 %s\n",
		tik->sig_type, tik->issuer
	);

	fprintf(stdout, "Signature:\n");
	hexdump(tik->signature, 0x100);
	fprintf(stdout, "\n");

	memdump(stdout, "Encrypted Titlekey:     ", tik->encrypted_title_key, 0x10);
	tik_decrypt_titlekey(tik, dtk);
	memdump(stdout, "Decrypted Titlekey:     ", dtk, 0x10);

	memdump(stdout,	"Ticket ID:              ", tik->ticket_id, 0x08);
	fprintf(stdout, "Ticket Version:         %d\n", tik->ticket_version);
	memdump(stdout,	"Title ID:               ", tik->title_id, 0x08);
	fprintf(stdout, "Common Key Index:       %d\n", tik->commonkey_idx);

	fprintf(stdout, "Content permission map:\n");
	for(i = 0; i < 0x40; i++) {
		printf(" %02x", tik->content_permissions[i]);

		if ((i+1) % 8 == 0)
			printf("\n");
	}
	printf("\n");
}
