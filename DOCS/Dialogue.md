# Dialogue System Outline

## Definitions

### Dialogue
A dialogue/dialogue tree, is a series of text interactions you have with the characters.

Dialogues have unique IDs (Dialogue-ID)
This is a 16-bit number

### Nodes
Each dialogue is broken into nodes/pages, which just represent a single back and forth
interaction. E.g.:

```
	Hello, how are you doing?

		Fine, thanks.
		Discombobulated.
	-->	Show me your tits. (bad-ending)
		Goodbye.
```

Each node has a unique ID (within its dialogue) (Node-ID)
This is a 16-bit number

Combined with its Dialogue ID, this makes a 32-bit ID (Full-Node-ID)
(Probably not necessary)

Dialogue nodes primarily contain the NPCs text, and a list of possible
responses, each of which is linked to another dialogue node.
The nodes also contain following information:
 - What character sprites to show, and their poses
 - What music should be playing
 - Conditional redirects (If a certain flag is set, jump to a different node)
(Should these just be handled with escapes?)

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
| `0x1B 00`       | Reset any previous style changes |
| `0x1B 01 pp pp` | Set text colour after sequence to the palette colour `0xpppp` |
| `0x1B 02 tt`    | Set text speed (delay) to `tt` ms per char (50 default) |
| `0x1B 03 tt`    | Wait `tt` ms before proceeding |
| `0x1B 10 pc`    | Set character sprite & pose; `p` is the pose number (0 = Not present); `c` is the character ID |
| `0x1B 20`       | Pause currently playing music |
| `0x1B 21`       | Unpause/Resume music |
| `0x1B 22 mm mm` | Change music to track with ID `0xmmmm` |
| `0x1B 2F ss ss` | Play sound effect with ID `0xssss` |

**Special Escape Sequence List**
| Escape Sequence | Effect |
|-----------------|--------|
| `0x1B E0 ff ff` | Clear Global flag `0xffff` (Set False) |
| `0x1B E1 ff ff` | Set Global flag `0xffff` (Set True) |
| `0x1B F0 nn nn` | Jump straight to node `0xnnnn` |

Question????????:
	How to handle multiple characters in a node?

## File Format
Each "page" of dialogue is stored as a single datablock.

Dialogues are a separate file?
Then master file of available dialogues?