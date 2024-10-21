//		
//			XML-Based Dialogue Compiler
//		
//		Takes a dialogue source file in XML and converts it into
//		the binary format that BHGH accepts.
//		
//		Compile with:
//			gcc -g -I'D:\Progs\c\sdl\gin-tonic\include' -L./.. -lmingw32 -lSDL2main -lSDL2 -lGinTonic -o xdc.exe XD.c xml.c
//		
//		Available Tags:
//			<dialogue root="">	Should be the root node; set the dialogue tree's inital node with root attribute
//			<node id="">	Dialogue node; Set this node's ID with `id`
//			<t>				Standard text element
//			<e>				Emphasised text
//			<gay>			*Very* emphasised text
//			<clr id="">		Text within this tag with be `id` coloured (draws from program palette)
//			<b>				Insert Bytes directly; If an escape isn't directly supported yet, just use this
//							Every pair of characters is interpreted as a single hexadecimal byte
//			<sp />			A single space (useful to insert between nodes)
//			<br />			A newline character.
//			<resp next="">	Response; Set next node ID with `next` (defaults to 0x0000/End Dialogue)
//		
//		To Do:

#include <stdio.h>
#include <stdlib.h>
#include <datablock.h>

#include "xml.h"


int get_string_val(struct xml_string *str, char *buf);
int get_node_name(struct xml_node *node, char *buf);
int get_node_content(struct xml_node *node, char *buf);
int get_attr_value(struct xml_node *node, char *buf, const char *attr_name);
void print_err(char *msg);
void print_help();


static char *src_file = NULL;
static char *dst_file = NULL;
static int line_nr = 1;
static int col_nr = 1;


int main(int argc, char *argv[]) {
	if (argc < 3) {
		puts("At least 2 arguments are required:");
		print_help();
		return 1;
	}
	src_file = argv[1];
	dst_file = argv[2];

	// Testing XML
	FILE *f_src = fopen(src_file, "rb");
	struct xml_document *src_doc = xml_open_document(f_src);
	if (src_doc == 0) {
		printf("Failed to open source file '%s'; Aborting...\n", src_file);
		return 1;
	}

	uint8_t data[0x400];
	int data_index = 0;
	char buf[0x400];

	Datablock *datablock_buf[0x100];
	int num_datablocks = 0;

	struct xml_node *root = xml_document_root(src_doc);
	size_t num_nodes = xml_node_children(root);
	// TODO: Check root node name

	// Parse Dialogue Root Attributes
	Uint16 root_node_id = 0x0001;
	int len = get_attr_value(root, buf, "root");
	if (len < 0) {
		printf("Warning: No root node set; defaulting to 0x0001...\n");
	} else {
		root_node_id = strtol(buf, NULL, 0);
	}

	Uint16 pose_table = 0x0000;
	len = get_attr_value(root, buf, "poses");
	if (len < 0) {
		printf("Warning: No poses set; defaulting to 0x0000. (No NPCs shown)\n");
	} else {
		pose_table = strtol(buf, NULL, 0);
	}

	Uint16 bg_music = 0x0000;
	len = get_attr_value(root, buf, "music");
	if (len < 0) {
		printf("Warning: No Music set; defaulting to 0x0000. (No Music)\n");
	} else {
		bg_music = strtol(buf, NULL, 0);
	}

	data[0] = root_node_id & 0xFF;
	data[1] = (root_node_id >> 8) & 0xFF;
	data[2] = (pose_table >> 8) & 0xFF;		// Yes, *one* of the header words is big-endian. I made this whole project in two weeks.
	data[3] = pose_table & 0xFF;
	data[4] = bg_music & 0xFF;
	data[5] = (bg_music >> 8) & 0xFF;
	data[6] = 0x00;	// Unused header bytes
	data[7] = 0x00;
	datablock_buf[0] = Datablock_Create(0x0000, data, 8);
	num_datablocks++;
	data_index = 0;

	for (size_t ni=0; ni<num_nodes; ni++) {
		struct xml_node *node = xml_node_child(root, ni);
		// TODO: Check node names 'node'

		Uint16 node_id = 0x0001;
		int len = get_attr_value(node, buf, "id");
		if (len < 0) {
			printf("Error: Node %i has no ID set!\n", num_datablocks);
			continue;
		} else {
			node_id = strtol(buf, NULL, 0);
		}

		data_index = 0;
		int text_len = 0;
		data[data_index++] = 0x00;
		data[data_index++] = 0x00;
		data[data_index++] = 0x00;
		data[data_index++] = 0x00;

		size_t num_text_nodes = xml_node_children(node);
		for (size_t tni=0; tni<num_text_nodes; tni++) {
			struct xml_node *text_node = xml_node_child(node, tni);
			char tn_tag[32];
			get_node_name(text_node, tn_tag);
		
			if (strncmp(tn_tag, "t", 32) == 0) {
				int len = get_node_content(text_node, data+data_index);
				data_index += len;
			} else if (strncmp(tn_tag, "e", 32) == 0) {
				data[data_index++] = 0x1B;
				data[data_index++] = 0x01;
				int len = get_node_content(text_node, data+data_index);
				data_index += len;
				data[data_index++] = 0x1B;
				data[data_index++] = 0x00;
			} else if (strncmp(tn_tag, "gay", 32) == 0) {
				data[data_index++] = 0x1B;
				data[data_index++] = 0x0F;
				int len = get_node_content(text_node, data+data_index);
				data_index += len;
				data[data_index++] = 0x1B;
				data[data_index++] = 0x00;
			} else if (strncmp(tn_tag, "sp", 32) == 0) {
				data[data_index++] = 0x20;
			} else if (strncmp(tn_tag, "br", 32) == 0) {
				data[data_index++] = 0x0A;
			} else if (strncmp(tn_tag, "b", 32) == 0) {
				char byte_str[256];
				int len = get_node_content(text_node, byte_str);
				if (len < 0) continue;

				for (int i=0; i<len; i+=2) {
					char single[3] = {
						byte_str[i+0],
						byte_str[i+1],
						0x00,
					};
					data[data_index++] = strtol(single, NULL, 16);
				}

			} else if (strncmp(tn_tag, "clr", 32) == 0) {
				char clr_buf[64];
				int len = get_attr_value(text_node, clr_buf, "id");
				if (len < 0) {
					// If no clr id is specified, assume reset
					data[data_index++] = 0x1B;
					data[data_index++] = 0x00;
					int len = get_node_content(text_node, data+data_index);
					data_index += len;
					continue;
				}

				Uint16 clr = strtol(clr_buf, NULL, 0);
				data[data_index++] = 0x1B;
				data[data_index++] = 0x02;
				data[data_index++] = (clr >> 8) & 0xFF;
				data[data_index++] = clr & 0xFF;
				len = get_node_content(text_node, data+data_index);
				data_index += len;
				data[data_index++] = 0x1B;
				data[data_index++] = 0x00;
			} else if (strncmp(tn_tag, "resp", 32) == 0) {
				if (text_len == 0) text_len = data_index-4;

				char resp_buf[64];
				int len = get_attr_value(text_node, resp_buf, "next");
				if (len < 0) {
					// If no next is specified, assume exit
					data[data_index++] = 0x00;
					data[data_index++] = 0x00;
				} else {
					Uint16 next_node = strtol(resp_buf, NULL, 0);
					data[data_index++] = next_node & 0xFF;
					data[data_index++] = (next_node >> 8) & 0xFF;
				}

				len = get_node_content(text_node, resp_buf);
				if (len < 0) {
					data[data_index++] = 0x00;
					data[data_index++] = 0x00; // Unused response-header byte
				} else {
					data[data_index++] = len & 0xFF;
					data[data_index++] = 0x00; // Unused response-header byte
					memcpy(data+data_index, resp_buf, len);
					data_index += len;
				}
			}
		}

		data[0] = text_len & 0xFF;
		datablock_buf[num_datablocks++] = Datablock_Create(node_id, data, data_index);
	}
	xml_document_free(src_doc, true);

	Datablock_File *dbf = Datablock_File_Create(dst_file, datablock_buf, num_datablocks);
	Datablock_File_Close(dbf);

	printf("Successfully created dialogue file '%s'\n", dst_file);

	return 0;
}


int get_string_val(struct xml_string *str, char *buf) {
	size_t len = xml_string_length(str);
	xml_string_copy(str, buf, len);
	buf[len] = '\0';
	return len;
}

int get_node_name(struct xml_node *node, char *buf) {
	struct xml_string *node_name = xml_node_name(node);
	return get_string_val(node_name, buf);
}

int get_node_content(struct xml_node *node, char *buf) {
	struct xml_string *node_cont = xml_node_content(node);
	return get_string_val(node_cont, buf);
}

int get_attr_value(struct xml_node *node, char *buf, const char *attr_name) {
	size_t attrs = xml_node_attributes(node);
	char tmp[256];
	for (int i=0; i<attrs; i++) {
		struct xml_string *aname = xml_node_attribute_name(node, i);
		size_t len = xml_string_length(aname);
		xml_string_copy(aname, tmp, 256);
		tmp[len] = '\0';

		if (strncmp(tmp, attr_name, 256) == 0) {
			struct xml_string *attr_content = xml_node_attribute_content(node, i);
			len = xml_string_length(attr_content);
			xml_string_copy(attr_content, buf, len);
			buf[len] = '\0';
			return len;
		}
	}

	return -1;
}

void print_err(char *msg) {
	printf("%s:%i:%i: %s\n", src_file, line_nr, col_nr, msg);
}

void print_help() {
	printf(
		"\n"
		"Usage:\n"
		"  xdc.exe <source-file> <dest-file>\n"
	);
}