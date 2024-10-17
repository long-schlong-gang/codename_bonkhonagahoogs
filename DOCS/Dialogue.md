# Dialogue System Outline

## Definitions

### Dialogue
A dialogue/dialogue tree, is a series of text interactions you have with the characters.

Dialogues have unique IDs (Dialogue-ID)
This is a 16-bit number

### Nodes
Each dialogue is broken into nodes, which just represent a single back and forth
interaction. E.g.:

```
	Hello, how are you doing?

		Fine, thanks.
		Discombobulated.
	-->	Show me your tits. (bad-ending)
		Goodbye.
```

Each node has a unique ID (within its dialogue) (Node-ID)
This is a 16-bit number (Cannot be 0x0000)

Combined with its Dialogue ID, this makes a 32-bit ID (Full-Node-ID)
(Probably not necessary)

Dialogue nodes primarily contain the NPCs text, and a list of possible
responses, each of which is linked to another dialogue node.
The nodes also contain following information:
 - What character sprites to show, and their poses
 - What music should be playing
 - Conditional redirects (If a certain flag is set, jump to a different node)
(Should these just be handled with escapes?)

~~The text of a node may be split into "pages", that is a longer stretch of		~~
~~text separated by the ASCII "Group Separator" (`GS`) character `0x1D`.     	~~
~~These will cause the text to stop and display a "next" character (->), while	~~
~~waiting for the player to press a key.                                     	~~

### Character Text
Each node contains text spoken by the other character in the dialogue.
This may contain special escape sequences for:
 - Changing text style (colour)
 - Adding pauses in typed out text
 - Start/stop music/sound effects
 - Set/Clear global flags

### Player Responses
Each node also contains a list of possible player responses.
These contain:
 - The response text (may also include escapes)
 - The ID of the node to jump to if this response is selected


## Text Escape Sequences
Within the text of a dialogue node (and the player's responses (?)),
certain sequences of characters may be used to add more depth to the dialogue.
Note: `0x1B` is `ESC` in ASCII

**Presentation Escape Sequence List**
| Escape Sequence | Effect |
|-----------------|--------|
| `0x1B 00`       | Reset text to usual colour |
| `0x1B 01`       | Emphasise following text; Equivalent to `0x1B 02 00 05` (Set colour to CLR_TEXT_EMPH) |
| `0x1B 02 pp pp` | Set text colour after sequence to the palette colour `0xpppp` |
| `0x1B 03 tt`    | Set text speed (delay) to `tt` ms per char (50 default) |
| `0x1B 04 tt`    | Wait `tt` ms before proceeding |
| `0x1B 10 cs`    | Set character sprite/pose; `s` is the pose/sprite number (0 = Not present); `c` is the character ID |
| `0x1B 11 cx`    | Set character position; `x` is the signed horizontal position (see below); `c` is the character ID |
| `0x1B 20`       | Pause currently playing music |
| `0x1B 21`       | Unpause/Resume music |
| `0x1B 22 mm mm` | Change music to track with ID `0xmmmm` |
| `0x1B 2F ss ss` | Play sound effect with ID `0xssss` |

// Put non-text escapes in separate kind of "script" string??
// Maybe separated with special ASCII control characters?
// --> Avoid using special ASCII where possible, as it might cause issues when transmitting dialogue files!
// Alternative: You've spent that time getting UTF-8 working; use special characters
// That won't be displayed/You won't use otherwise: How about:
//	
//	"*dialogue with story implications that changes the mood of the scene*"
//	"𝄆0001𝄇" --> Start playing music 0x0001
//	"𝄞\x0001" --> Start playing music 0x0001, (♫ ?)
//	"𝆓" --> Decrescendo; Fade music out
//	"ᛗ" --> M; music?
//	"※\x0001" --> Explosion; play sound effect 0x0001?
//	"⏳\x10" --> Wait 10ms
//	"☺\x13" --> Set character 1's expression/pose to nr. 3
//	"Simple ⚑FLAGS⚐ to indicate emphasis?"
//	"✍\x0000" --> Set colour
//	"*...*"
//	See Unicode Block 0x25A0 and up for more

**Special Escape Sequence List**
| Escape Sequence | Effect |
|-----------------|--------|
| `0x1B E0 ff ff` | Clear Global flag `0xffff` (Set False) |
| `0x1B E1 ff ff` | Set Global flag `0xffff` (Set True) |
| `0x1B F0 nn nn` | Jump straight to node `0xnnnn` |

**Character IDs**
| ID  | Hex | Character  |
|-----|-----|------------|
| `0` | `0` | NULL/None  |
| `1` | `1` | Levu       |
| `2` | `2` | Eruya      |
| `3` | `3` | Fedelov    |
| `4` | `4` | Kelen      |
| ... | ... | UNUSED     |
| `15`| `F` | You/Player |

Question????????:
	How to handle multiple characters in a node?

## File Format
Each node of dialogue is stored as a single datablock.
The datablock's DBID is just the Node-ID

A dialogue file has a single header block as the first block in the file.
This contains following fields: (little-endian (System native))

| Byte Pos. | Length (B) | Field Description |
|-----------|------------|-------------------|
| `0x0000`  | 16-Bit, 2B | Root Node block ID |
| `0x0002`  | 16-Bit, 2B | Reserved for future use |

Each node block contains the following fields: (little-endian (System native))

| Byte Pos. | Length (B) | Field Description |
|-----------|------------|-------------------|
| `0x0000`  | 8-Bit,  1B | Length of following Dialogue Text in Bytes --> `len` |
| `0x0001`  | 24-Bit, 3B | Reserved Header bytes for extra flags; Ignored |
| `0x0004`  | (var.), nB | UTF-8 Encoded Dialogue Text; `len` bytes long exactly |

After the dialogue text comes an array of possible responses;
if it only consists of a single Node-ID, this is considered the
next page.

| Byte Pos. | Length (B) | Field Description |
|-----------|------------|-------------------|
| `0x0000`  | 16-Bit, 2B | Node-/Block-ID this Response leads to; If it's 0x0000, that ends the dialogue |
| `0x0002`  | 8-Bit,  1B | Length of following Response text in bytes --> `len` |
| `0x0003`  | 8-Bit,  1B | Reserved for Response Flags |
| `0x0004`  | (var.), nB | Response Text; exactly `len` bytes |


Dialogues are a separate file?
...then master file of available dialogues?
YES!

