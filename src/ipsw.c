/* */
#include <stdio.h>
#include <plist/plist.h>
#include <zip.h>
#include "ipsw.h"
#include "st0rm.h"

void check_ipsw(const char *s_ipsw)
{
	plist_t bman=NULL;
	char *ios_v=NULL;
	char *device_target=NULL;
	char *target=NULL;

	int e=ipsw_extract_build_manifest(s_ipsw, &bman);

	if (e==0)
	{
		printf("INFO: Got the build manifest\n");

		plist_t bident=build_manifest_get_build_identity(bman);

		printf("INFO: Got the build identity\n");

		plist_t targets_array=plist_dict_get_item(bman, "SupportedProductTypes");
		if (targets_array!=NULL)
		{
			plist_t target_node=plist_array_get_item(targets_array, 0);

			if (target_node!=NULL)
			{
				plist_get_string_val(target_node, &device_target);
			}

			else {
				printf("ERROR: Could not get the target :(\n");
			}
		}

		else {
			printf("ERROR: Could not get the array of targets :(\n");
		}

		plist_t node=plist_dict_get_item(bman, "ProductVersion");
		plist_get_string_val(node, &ios_v);
		plist_free(node);
		node=NULL;

		char *target_str[512];
		snprintf(target_str, 512, "%s_%s", device_target, ios_v);

		if (strcmp(target_str, "iPhone1,1_3.1.3")==0)
		{
			pwnable=1;
			target="m68ap";
			hr_target_str="iPhone2G 3.1.3";
		}

		else if (strcmp(target_str, "iPhone1,2_4.0")==0)
		{
			pwnable=1;
			target="n82ap";
			hr_target_str="iPhone3G 4.0";
		}

		else if (strcmp(target_str, "iPhone2,1_4.0")==0)
		{
			pwnable=0;
			target="";
			hr_target_str="iPhone3GS 4.0";
			// TODO: Add dialog to check for old bootrom
			info("Your iPhone 3GS must be already jailbroken to use Starst0rm.\nIf this model has the new bootrom, it cannot be jailbroken with Starst0rm.", "This is important...");
		}

		else if (strcmp(target_str, "iPod1,1_3.1.3")==0)
		{
			pwnable=1;
			target="n82ap";
			hr_target_str="iPod Touch 1G 3.1.3";

			// No baseband here :P
			bb_can_up=0;
		}

		else if (strcmp(target_str, "iPod2,1_4.0")==0)
		{
			pwnable=0;
			target="n72ap";
			hr_target_str="iPod Touch 2G 4.0";

			// Or here :P
			bb_can_up=0;

			// TODO: Add dialog to check for old bootrom
			info("Your iPod Touch (2nd Generation) must be already jailbroken to use Starst0rm.\nIf this is an MC model, it cannot be jailbroken with Starst0rm.", "This is important...");
		}

		else {
			printf("INFO: %d\n", strcmp(target_str, "iPod2,1_4.0")==0);
			info("The ipsw you selected is invalid!\nThe program will now exit...", "Fatal Error!");
			exit(-1);
		}
		//strcpy(target, (char *)target_str);
	}

	else {
		printf("ERROR: %d\n", e);
		info("The ipsw you selected is invalid!\nThe program will now exit...", "Fatal Error!");
		exit(-1);
	}
}

// This function is originally from p0sixninja's idevicerestore (modded a bit by me, though ;P)
int ipsw_extract_build_manifest(const char* ipsw, plist_t* buildmanifest) {
	int size = 0;
	char* data = NULL;

	/* older devices don't require personalized firmwares and use a BuildManifesto.plist */
	if (ipsw_extract_to_memory(ipsw, "BuildManifesto.plist", &data, &size) == 0) {
		plist_from_xml(data, size, buildmanifest);
		return 0;
	}

	//data = NULL;
	//size = 0;

	/* whereas newer devices do not require personalized firmwares and use a BuildManifest.plist */
	if (ipsw_extract_to_memory(ipsw, "BuildManifest.plist", &data, &size) == 0) {
		plist_from_xml(data, size, buildmanifest);
		return 0;
	}

	return -1;
}

plist_t build_manifest_get_build_identity(plist_t build_manifest) {

	// Always fetch the first one...
	uint32_t identity=1;

	// fetch build identities array from BuildManifest
	plist_t build_identities_array = plist_dict_get_item(build_manifest, "BuildIdentities");
	if (!build_identities_array || plist_get_node_type(build_identities_array) != PLIST_ARRAY) {
		error("ERROR: Unable to find build identities node\n");
		return NULL;
	}

	// check and make sure this identity exists in buildmanifest
	if (identity >= plist_array_get_size(build_identities_array)) {
		return NULL;
	}

	plist_t build_identity = plist_array_get_item(build_identities_array, identity);
	if (!build_identity || plist_get_node_type(build_identity) != PLIST_DICT) {
		error("ERROR: Unable to find build identities node\n");
		return NULL;
	}

	return plist_copy(build_identity);
}

ipsw_archive* ipsw_open(const char* ipsw) {
	int err = 0;
	ipsw_archive* archive = (ipsw_archive*) malloc(sizeof(ipsw_archive));
	if (archive == NULL) {
		printf("ERROR: Out of memory\n");
		return NULL;
	}

	archive->zip = zip_open(ipsw, 0, &err);
	if (archive->zip == NULL) {
		printf("ERROR: zip_open: %s: %d\n", ipsw, err);
		free(archive);
		return NULL;
	}

	return archive;
}

void ipsw_close(ipsw_archive* archive) {
	if (archive != NULL) {
		zip_unchange_all(archive->zip);
		zip_close(archive->zip);
		free(archive);
	}
}

int ipsw_extract_to_memory(const char* ipsw, const char* infile, char** pbuffer, uint32_t* psize) {
	ipsw_archive* archive = ipsw_open(ipsw);
	if (archive == NULL || archive->zip == NULL) {
		printf("ERROR: Invalid archive\n");
		return -1;
	}

	int zindex = zip_name_locate(archive->zip, infile, 0);
	if (zindex < 0) {
		printf("ERROR: zip_name_locate: %s\n", infile);
		return -1;
	}

	struct zip_stat zstat;
	zip_stat_init(&zstat);
	if (zip_stat_index(archive->zip, zindex, 0, &zstat) != 0) {
		printf("ERROR: zip_stat_index: %s\n", infile);
		return -1;
	}

	struct zip_file* zfile = zip_fopen_index(archive->zip, zindex, 0);
	if (zfile == NULL) {
		printf("ERROR: zip_fopen_index: %s\n", infile);
		return -1;
	}

	int size = zstat.size;
	char* buffer = (unsigned char*) malloc(size);
	if (buffer == NULL) {
		printf("ERROR: Out of memory\n");
		zip_fclose(zfile);
		return -1;
	}

	if (zip_fread(zfile, buffer, size) != size) {
		printf("ERROR: zip_fread: %s\n", infile);
		zip_fclose(zfile);
		free(buffer);
		return -1;
	}

	zip_fclose(zfile);
	ipsw_close(archive);

	*pbuffer = buffer;
	*psize = size;
	return 0;
}
