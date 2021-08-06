/*
Blendish - Blender 2.5 UI based theming functions for NanoVG

Copyright (c) 2014 Leonard Ritter <leonard.ritter@duangle.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef BLENDISH_H
#define BLENDISH_H

#include <nanovg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*

Revision 6 (2014-09-21)

Summary
-------

Blendish is a small collection of drawing functions for NanoVG, designed to 
replicate the look of the Blender 2.5+ User Interface. You can use these 
functions to theme your UI library. Several metric constants for faithful
reproduction are also included.

Blendish supports the original Blender icon sheet; As the licensing of Blenders
icons is unclear, they are not included in Blendishes repository, but a SVG
template, "icons_template.svg" is provided, which you can use to build your own
icon sheet.

To use icons, you must first load the icon sheet using one of the 
nvgCreateImage*() functions and then pass the image handle to bndSetIconImage();
otherwise, no icons will be drawn. See bndSetIconImage() for more information.

Blendish will not render text until a suitable UI font has been passed to
bndSetFont() has been called. See bndSetFont() for more information.


Drawbacks
---------

There is no support for varying dpi resolutions yet. The library is hardcoded
to the equivalent of 72 dpi in the Blender system settings.

Support for label truncation is missing. Text rendering breaks when widgets are
too short to contain their labels.

Usage
-----

To use this header file in implementation mode, define BLENDISH_IMPLEMENTATION
before including blendish.h, otherwise the file will be in header-only mode.

*/

// you can override this from the outside to pick
// the export level you need
#ifndef BND_EXPORT
#define BND_EXPORT
#endif

// if that typedef is provided elsewhere, you may define 
// BLENDISH_NO_NVG_TYPEDEFS before including the header.
#ifndef BLENDISH_NO_NVG_TYPEDEFS
typedef struct NVGcontext NVGcontext;
typedef struct NVGcolor NVGcolor;
typedef struct NVGglyphPosition NVGglyphPosition;
#endif

// describes the theme used to draw a single widget or widget box;
// these values correspond to the same values that can be retrieved from
// the Theme panel in the Blender preferences
typedef struct BNDwidgetTheme {
    // color of widget box outline
    NVGcolor outlineColor;
    // color of widget item (meaning changes depending on class)
    NVGcolor itemColor;
    // fill color of widget box
    NVGcolor innerColor;
    // fill color of widget box when active
    NVGcolor innerSelectedColor;
    // color of text label
    NVGcolor textColor;
    // color of text label when active
    NVGcolor textSelectedColor;
    // delta modifier for upper part of gradient (-100 to 100)
    int shadeTop;
    // delta modifier for lower part of gradient (-100 to 100)
    int shadeDown;
} BNDwidgetTheme;

// describes the theme used to draw nodes
typedef struct BNDnodeTheme {
    // inner color of selected node (and downarrow)
    NVGcolor nodeSelectedColor;
    // outline of wires
    NVGcolor wiresColor;
    // color of text label when active
    NVGcolor textSelectedColor;
    
    // inner color of active node (and dragged wire)
    NVGcolor activeNodeColor;
    // color of selected wire
    NVGcolor wireSelectColor;
    // color of background of node
    NVGcolor nodeBackdropColor;
    
    // how much a noodle curves (0 to 10)
    int noodleCurving;
} BNDnodeTheme;

// describes the theme used to draw widgets
typedef struct BNDtheme {
    // the background color of panels and windows
    NVGcolor backgroundColor;
    // theme for labels
    BNDwidgetTheme regularTheme;
    // theme for tool buttons
    BNDwidgetTheme toolTheme;
    // theme for radio buttons
    BNDwidgetTheme radioTheme;
    // theme for text fields
    BNDwidgetTheme textFieldTheme;
    // theme for option buttons (checkboxes)
    BNDwidgetTheme optionTheme;
    // theme for choice buttons (comboboxes)
    // Blender calls them "menu buttons"
    BNDwidgetTheme choiceTheme;
    // theme for number fields
    BNDwidgetTheme numberFieldTheme;
    // theme for slider controls
    BNDwidgetTheme sliderTheme;
    // theme for scrollbars
    BNDwidgetTheme scrollBarTheme;
    // theme for tooltips
    BNDwidgetTheme tooltipTheme;
    // theme for menu backgrounds
    BNDwidgetTheme menuTheme;
    // theme for menu items
    BNDwidgetTheme menuItemTheme;
    // theme for nodes
    BNDnodeTheme nodeTheme;
} BNDtheme;

// how text on a control is aligned
typedef enum BNDtextAlignment {
    BND_LEFT = 0,
    BND_CENTER,
} BNDtextAlignment;

// states altering the styling of a widget
typedef enum BNDwidgetState {
    // not interacting
    BND_DEFAULT = 0,
    // the mouse is hovering over the control
    BND_HOVER,
    // the widget is activated (pressed) or in an active state (toggled)
    BND_ACTIVE
} BNDwidgetState;

// flags indicating which corners are sharp (for grouping widgets)
typedef enum BNDcornerFlags {
    // all corners are round
    BND_CORNER_NONE = 0,
    // sharp top left corner
    BND_CORNER_TOP_LEFT = 1,
    // sharp top right corner
    BND_CORNER_TOP_RIGHT = 2,
    // sharp bottom right corner
    BND_CORNER_DOWN_RIGHT = 4,
    // sharp bottom left corner
    BND_CORNER_DOWN_LEFT = 8,
    // all corners are sharp; 
    // you can invert a set of flags using ^= BND_CORNER_ALL
    BND_CORNER_ALL = 0xF,
    // top border is sharp
    BND_CORNER_TOP = 3,
    // bottom border is sharp
    BND_CORNER_DOWN = 0xC,
    // left border is sharp
    BND_CORNER_LEFT = 9,
    // right border is sharp
    BND_CORNER_RIGHT = 6
} BNDcornerFlags;

// build an icon ID from two coordinates into the icon sheet, where
// (0,0) designates the upper-leftmost icon, (1,0) the one right next to it,
// and so on.
#define BND_ICONID(x,y) ((x)|((y)<<8))
// alpha of disabled widget groups
// can be used in conjunction with nvgGlobalAlpha()
#define BND_DISABLED_ALPHA 0.5

// default widget height
#define BND_WIDGET_HEIGHT 21
// default toolbutton width (if icon only)
#define BND_TOOL_WIDTH 20

// default radius of node ports
#define BND_NODE_PORT_RADIUS 5
// top margin of node content
#define BND_NODE_MARGIN_TOP 25
// bottom margin of node content
#define BND_NODE_MARGIN_DOWN 5
// left and right margin of node content
#define BND_NODE_MARGIN_SIDE 10
// height of node title bar
#define BND_NODE_TITLE_HEIGHT 20
// width of node title arrow click area
#define BND_NODE_ARROW_AREA_WIDTH 20

// size of splitter corner click area
#define BND_SPLITTER_AREA_SIZE 12

// width of vertical scrollbar
#define BND_SCROLLBAR_WIDTH 13
// height of horizontal scrollbar
#define BND_SCROLLBAR_HEIGHT 14

// default vertical spacing
#define BND_VSPACING 1
// default vertical spacing between groups
#define BND_VSPACING_GROUP 8
// default horizontal spacing
#define BND_HSPACING 8

// default text size
#define BND_LABEL_FONT_SIZE 13

// default text padding in inner box
#define BND_PAD_LEFT 8
#define BND_PAD_RIGHT 8

// label: value separator string
#define BND_LABEL_SEPARATOR ": "

// alpha intensity of transparent items (0xa4)
#define BND_TRANSPARENT_ALPHA 0.9

// shade intensity of beveled panels
#define BND_BEVEL_SHADE 30
// shade intensity of beveled insets
#define BND_INSET_BEVEL_SHADE 30
// shade intensity of hovered inner boxes
#define BND_HOVER_SHADE 15
// shade intensity of splitter bevels
#define BND_SPLITTER_SHADE 100

// width of icon sheet
#define BND_ICON_SHEET_WIDTH 602
// height of icon sheet
#define BND_ICON_SHEET_HEIGHT 640
// gridsize of icon sheet in both dimensions
#define BND_ICON_SHEET_GRID 21
// offset of first icon tile relative to left border
#define BND_ICON_SHEET_OFFSET_X 5
// offset of first icon tile relative to top border
#define BND_ICON_SHEET_OFFSET_Y 10
// resolution of single icon
#define BND_ICON_SHEET_RES 16

// size of number field arrow
#define BND_NUMBER_ARROW_SIZE 4

// default text color
#define BND_COLOR_TEXT {{{ 0,0,0,1 }}}
// default highlighted text color
#define BND_COLOR_TEXT_SELECTED {{{ 1,1,1,1 }}}

// radius of tool button
#define BND_TOOL_RADIUS 4

// radius of option button
#define BND_OPTION_RADIUS 4
// width of option button checkbox
#define BND_OPTION_WIDTH 14
// height of option button checkbox
#define BND_OPTION_HEIGHT 15

// radius of text field
#define BND_TEXT_RADIUS 4

// radius of number button
#define BND_NUMBER_RADIUS 10

// radius of menu popup
#define BND_MENU_RADIUS 3
// feather of menu popup shadow
#define BND_SHADOW_FEATHER 12
// alpha of menu popup shadow
#define BND_SHADOW_ALPHA 0.5

// radius of scrollbar
#define BND_SCROLLBAR_RADIUS 7
// shade intensity of active scrollbar
#define BND_SCROLLBAR_ACTIVE_SHADE 15

// max glyphs for position testing
#define BND_MAX_GLYPHS 1024

// max rows for position testing
#define BND_MAX_ROWS 32

// text distance from bottom
#define BND_TEXT_PAD_DOWN 7

// stroke width of wire outline
#define BND_NODE_WIRE_OUTLINE_WIDTH 4
// stroke width of wire
#define BND_NODE_WIRE_WIDTH 2
// radius of node box
#define BND_NODE_RADIUS 8
// feather of node title text
#define BND_NODE_TITLE_FEATHER 1
// size of node title arrow
#define BND_NODE_ARROW_SIZE 9

typedef enum BNDicon {
	BND_ICON_NONE = BND_ICONID(0,29),
	BND_ICON_QUESTION = BND_ICONID(1,29),
	BND_ICON_ERROR = BND_ICONID(2,29),
	BND_ICON_CANCEL = BND_ICONID(3,29),
	BND_ICON_TRIA_RIGHT = BND_ICONID(4,29),
	BND_ICON_TRIA_DOWN = BND_ICONID(5,29),
	BND_ICON_TRIA_LEFT = BND_ICONID(6,29),
	BND_ICON_TRIA_UP = BND_ICONID(7,29),
	BND_ICON_ARROW_LEFTRIGHT = BND_ICONID(8,29),
	BND_ICON_PLUS = BND_ICONID(9,29),
	BND_ICON_DISCLOSURE_TRI_DOWN = BND_ICONID(10,29),
	BND_ICON_DISCLOSURE_TRI_RIGHT = BND_ICONID(11,29),
	BND_ICON_RADIOBUT_OFF = BND_ICONID(12,29),
	BND_ICON_RADIOBUT_ON = BND_ICONID(13,29),
	BND_ICON_MENU_PANEL = BND_ICONID(14,29),
	BND_ICON_BLENDER = BND_ICONID(15,29),
	BND_ICON_GRIP = BND_ICONID(16,29),
	BND_ICON_DOT = BND_ICONID(17,29),
	BND_ICON_COLLAPSEMENU = BND_ICONID(18,29),
	BND_ICON_X = BND_ICONID(19,29),
	BND_ICON_GO_LEFT = BND_ICONID(21,29),
	BND_ICON_PLUG = BND_ICONID(22,29),
	BND_ICON_UI = BND_ICONID(23,29),
	BND_ICON_NODE = BND_ICONID(24,29),
	BND_ICON_NODE_SEL = BND_ICONID(25,29),

	BND_ICON_FULLSCREEN = BND_ICONID(0,28),
	BND_ICON_SPLITSCREEN = BND_ICONID(1,28),
	BND_ICON_RIGHTARROW_THIN = BND_ICONID(2,28),
	BND_ICON_BORDERMOVE = BND_ICONID(3,28),
	BND_ICON_VIEWZOOM = BND_ICONID(4,28),
	BND_ICON_ZOOMIN = BND_ICONID(5,28),
	BND_ICON_ZOOMOUT = BND_ICONID(6,28),
	BND_ICON_PANEL_CLOSE = BND_ICONID(7,28),
	BND_ICON_COPY_ID = BND_ICONID(8,28),
	BND_ICON_EYEDROPPER = BND_ICONID(9,28),
	BND_ICON_LINK_AREA = BND_ICONID(10,28),
	BND_ICON_AUTO = BND_ICONID(11,28),
	BND_ICON_CHECKBOX_DEHLT = BND_ICONID(12,28),
	BND_ICON_CHECKBOX_HLT = BND_ICONID(13,28),
	BND_ICON_UNLOCKED = BND_ICONID(14,28),
	BND_ICON_LOCKED = BND_ICONID(15,28),
	BND_ICON_UNPINNED = BND_ICONID(16,28),
	BND_ICON_PINNED = BND_ICONID(17,28),
	BND_ICON_SCREEN_BACK = BND_ICONID(18,28),
	BND_ICON_RIGHTARROW = BND_ICONID(19,28),
	BND_ICON_DOWNARROW_HLT = BND_ICONID(20,28),
	BND_ICON_DOTSUP = BND_ICONID(21,28),
	BND_ICON_DOTSDOWN = BND_ICONID(22,28),
	BND_ICON_LINK = BND_ICONID(23,28),
	BND_ICON_INLINK = BND_ICONID(24,28),
	BND_ICON_PLUGIN = BND_ICONID(25,28),

	BND_ICON_HELP = BND_ICONID(0,27),
	BND_ICON_GHOST_ENABLED = BND_ICONID(1,27),
	BND_ICON_COLOR = BND_ICONID(2,27),
	BND_ICON_LINKED = BND_ICONID(3,27),
	BND_ICON_UNLINKED = BND_ICONID(4,27),
	BND_ICON_HAND = BND_ICONID(5,27),
	BND_ICON_ZOOM_ALL = BND_ICONID(6,27),
	BND_ICON_ZOOM_SELECTED = BND_ICONID(7,27),
	BND_ICON_ZOOM_PREVIOUS = BND_ICONID(8,27),
	BND_ICON_ZOOM_IN = BND_ICONID(9,27),
	BND_ICON_ZOOM_OUT = BND_ICONID(10,27),
	BND_ICON_RENDER_REGION = BND_ICONID(11,27),
	BND_ICON_BORDER_RECT = BND_ICONID(12,27),
	BND_ICON_BORDER_LASSO = BND_ICONID(13,27),
	BND_ICON_FREEZE = BND_ICONID(14,27),
	BND_ICON_STYLUS_PRESSURE = BND_ICONID(15,27),
	BND_ICON_GHOST_DISABLED = BND_ICONID(16,27),
	BND_ICON_NEW = BND_ICONID(17,27),
	BND_ICON_FILE_TICK = BND_ICONID(18,27),
	BND_ICON_QUIT = BND_ICONID(19,27),
	BND_ICON_URL = BND_ICONID(20,27),
	BND_ICON_RECOVER_LAST = BND_ICONID(21,27),
	BND_ICON_FULLSCREEN_ENTER = BND_ICONID(23,27),
	BND_ICON_FULLSCREEN_EXIT = BND_ICONID(24,27),
	BND_ICON_BLANK1 = BND_ICONID(25,27),

	BND_ICON_LAMP = BND_ICONID(0,26),
	BND_ICON_MATERIAL = BND_ICONID(1,26),
	BND_ICON_TEXTURE = BND_ICONID(2,26),
	BND_ICON_ANIM = BND_ICONID(3,26),
	BND_ICON_WORLD = BND_ICONID(4,26),
	BND_ICON_SCENE = BND_ICONID(5,26),
	BND_ICON_EDIT = BND_ICONID(6,26),
	BND_ICON_GAME = BND_ICONID(7,26),
	BND_ICON_RADIO = BND_ICONID(8,26),
	BND_ICON_SCRIPT = BND_ICONID(9,26),
	BND_ICON_PARTICLES = BND_ICONID(10,26),
	BND_ICON_PHYSICS = BND_ICONID(11,26),
	BND_ICON_SPEAKER = BND_ICONID(12,26),
	BND_ICON_TEXTURE_SHADED = BND_ICONID(13,26),

	BND_ICON_VIEW3D = BND_ICONID(0,25),
	BND_ICON_IPO = BND_ICONID(1,25),
	BND_ICON_OOPS = BND_ICONID(2,25),
	BND_ICON_BUTS = BND_ICONID(3,25),
	BND_ICON_FILESEL = BND_ICONID(4,25),
	BND_ICON_IMAGE_COL = BND_ICONID(5,25),
	BND_ICON_INFO = BND_ICONID(6,25),
	BND_ICON_SEQUENCE = BND_ICONID(7,25),
	BND_ICON_TEXT = BND_ICONID(8,25),
	BND_ICON_IMASEL = BND_ICONID(9,25),
	BND_ICON_SOUND = BND_ICONID(10,25),
	BND_ICON_ACTION = BND_ICONID(11,25),
	BND_ICON_NLA = BND_ICONID(12,25),
	BND_ICON_SCRIPTWIN = BND_ICONID(13,25),
	BND_ICON_TIME = BND_ICONID(14,25),
	BND_ICON_NODETREE = BND_ICONID(15,25),
	BND_ICON_LOGIC = BND_ICONID(16,25),
	BND_ICON_CONSOLE = BND_ICONID(17,25),
	BND_ICON_PREFERENCES = BND_ICONID(18,25),
	BND_ICON_CLIP = BND_ICONID(19,25),
	BND_ICON_ASSET_MANAGER = BND_ICONID(20,25),

	BND_ICON_OBJECT_DATAMODE = BND_ICONID(0,24),
	BND_ICON_EDITMODE_HLT = BND_ICONID(1,24),
	BND_ICON_FACESEL_HLT = BND_ICONID(2,24),
	BND_ICON_VPAINT_HLT = BND_ICONID(3,24),
	BND_ICON_TPAINT_HLT = BND_ICONID(4,24),
	BND_ICON_WPAINT_HLT = BND_ICONID(5,24),
	BND_ICON_SCULPTMODE_HLT = BND_ICONID(6,24),
	BND_ICON_POSE_HLT = BND_ICONID(7,24),
	BND_ICON_PARTICLEMODE = BND_ICONID(8,24),
	BND_ICON_LIGHTPAINT = BND_ICONID(9,24),

	BND_ICON_SCENE_DATA = BND_ICONID(0,23),
	BND_ICON_RENDERLAYERS = BND_ICONID(1,23),
	BND_ICON_WORLD_DATA = BND_ICONID(2,23),
	BND_ICON_OBJECT_DATA = BND_ICONID(3,23),
	BND_ICON_MESH_DATA = BND_ICONID(4,23),
	BND_ICON_CURVE_DATA = BND_ICONID(5,23),
	BND_ICON_META_DATA = BND_ICONID(6,23),
	BND_ICON_LATTICE_DATA = BND_ICONID(7,23),
	BND_ICON_LAMP_DATA = BND_ICONID(8,23),
	BND_ICON_MATERIAL_DATA = BND_ICONID(9,23),
	BND_ICON_TEXTURE_DATA = BND_ICONID(10,23),
	BND_ICON_ANIM_DATA = BND_ICONID(11,23),
	BND_ICON_CAMERA_DATA = BND_ICONID(12,23),
	BND_ICON_PARTICLE_DATA = BND_ICONID(13,23),
	BND_ICON_LIBRARY_DATA_DIRECT = BND_ICONID(14,23),
	BND_ICON_GROUP = BND_ICONID(15,23),
	BND_ICON_ARMATURE_DATA = BND_ICONID(16,23),
	BND_ICON_POSE_DATA = BND_ICONID(17,23),
	BND_ICON_BONE_DATA = BND_ICONID(18,23),
	BND_ICON_CONSTRAINT = BND_ICONID(19,23),
	BND_ICON_SHAPEKEY_DATA = BND_ICONID(20,23),
	BND_ICON_CONSTRAINT_BONE = BND_ICONID(21,23),
	BND_ICON_CAMERA_STEREO = BND_ICONID(22,23),
	BND_ICON_PACKAGE = BND_ICONID(23,23),
	BND_ICON_UGLYPACKAGE = BND_ICONID(24,23),

	BND_ICON_BRUSH_DATA = BND_ICONID(0,22),
	BND_ICON_IMAGE_DATA = BND_ICONID(1,22),
	BND_ICON_FILE = BND_ICONID(2,22),
	BND_ICON_FCURVE = BND_ICONID(3,22),
	BND_ICON_FONT_DATA = BND_ICONID(4,22),
	BND_ICON_RENDER_RESULT = BND_ICONID(5,22),
	BND_ICON_SURFACE_DATA = BND_ICONID(6,22),
	BND_ICON_EMPTY_DATA = BND_ICONID(7,22),
	BND_ICON_SETTINGS = BND_ICONID(8,22),
	BND_ICON_RENDER_ANIMATION = BND_ICONID(9,22),
	BND_ICON_RENDER_STILL = BND_ICONID(10,22),
	BND_ICON_BOIDS = BND_ICONID(12,22),
	BND_ICON_STRANDS = BND_ICONID(13,22),
	BND_ICON_LIBRARY_DATA_INDIRECT = BND_ICONID(14,22),
	BND_ICON_GREASEPENCIL = BND_ICONID(15,22),
	BND_ICON_LINE_DATA = BND_ICONID(16,22),
	BND_ICON_GROUP_BONE = BND_ICONID(18,22),
	BND_ICON_GROUP_VERTEX = BND_ICONID(19,22),
	BND_ICON_GROUP_VCOL = BND_ICONID(20,22),
	BND_ICON_GROUP_UVS = BND_ICONID(21,22),
	BND_ICON_RNA = BND_ICONID(24,22),
	BND_ICON_RNA_ADD = BND_ICONID(25,22),

	BND_ICON_OUTLINER_OB_EMPTY = BND_ICONID(0,20),
	BND_ICON_OUTLINER_OB_MESH = BND_ICONID(1,20),
	BND_ICON_OUTLINER_OB_CURVE = BND_ICONID(2,20),
	BND_ICON_OUTLINER_OB_LATTICE = BND_ICONID(3,20),
	BND_ICON_OUTLINER_OB_META = BND_ICONID(4,20),
	BND_ICON_OUTLINER_OB_LAMP = BND_ICONID(5,20),
	BND_ICON_OUTLINER_OB_CAMERA = BND_ICONID(6,20),
	BND_ICON_OUTLINER_OB_ARMATURE = BND_ICONID(7,20),
	BND_ICON_OUTLINER_OB_FONT = BND_ICONID(8,20),
	BND_ICON_OUTLINER_OB_SURFACE = BND_ICONID(9,20),
	BND_ICON_OUTLINER_OB_SPEAKER = BND_ICONID(10,20),
	BND_ICON_RESTRICT_VIEW_OFF = BND_ICONID(19,20),
	BND_ICON_RESTRICT_VIEW_ON = BND_ICONID(20,20),
	BND_ICON_RESTRICT_SELECT_OFF = BND_ICONID(21,20),
	BND_ICON_RESTRICT_SELECT_ON = BND_ICONID(22,20),
	BND_ICON_RESTRICT_RENDER_OFF = BND_ICONID(23,20),
	BND_ICON_RESTRICT_RENDER_ON = BND_ICONID(24,20),

	BND_ICON_OUTLINER_DATA_EMPTY = BND_ICONID(0,19),
	BND_ICON_OUTLINER_DATA_MESH = BND_ICONID(1,19),
	BND_ICON_OUTLINER_DATA_CURVE = BND_ICONID(2,19),
	BND_ICON_OUTLINER_DATA_LATTICE = BND_ICONID(3,19),
	BND_ICON_OUTLINER_DATA_META = BND_ICONID(4,19),
	BND_ICON_OUTLINER_DATA_LAMP = BND_ICONID(5,19),
	BND_ICON_OUTLINER_DATA_CAMERA = BND_ICONID(6,19),
	BND_ICON_OUTLINER_DATA_ARMATURE = BND_ICONID(7,19),
	BND_ICON_OUTLINER_DATA_FONT = BND_ICONID(8,19),
	BND_ICON_OUTLINER_DATA_SURFACE = BND_ICONID(9,19),
	BND_ICON_OUTLINER_DATA_SPEAKER = BND_ICONID(10,19),
	BND_ICON_OUTLINER_DATA_POSE = BND_ICONID(11,19),

	BND_ICON_MESH_PLANE = BND_ICONID(0,18),
	BND_ICON_MESH_CUBE = BND_ICONID(1,18),
	BND_ICON_MESH_CIRCLE = BND_ICONID(2,18),
	BND_ICON_MESH_UVSPHERE = BND_ICONID(3,18),
	BND_ICON_MESH_ICOSPHERE = BND_ICONID(4,18),
	BND_ICON_MESH_GRID = BND_ICONID(5,18),
	BND_ICON_MESH_MONKEY = BND_ICONID(6,18),
	BND_ICON_MESH_CYLINDER = BND_ICONID(7,18),
	BND_ICON_MESH_TORUS = BND_ICONID(8,18),
	BND_ICON_MESH_CONE = BND_ICONID(9,18),
	BND_ICON_LAMP_POINT = BND_ICONID(12,18),
	BND_ICON_LAMP_SUN = BND_ICONID(13,18),
	BND_ICON_LAMP_SPOT = BND_ICONID(14,18),
	BND_ICON_LAMP_HEMI = BND_ICONID(15,18),
	BND_ICON_LAMP_AREA = BND_ICONID(16,18),
	BND_ICON_META_EMPTY = BND_ICONID(19,18),
	BND_ICON_META_PLANE = BND_ICONID(20,18),
	BND_ICON_META_CUBE = BND_ICONID(21,18),
	BND_ICON_META_BALL = BND_ICONID(22,18),
	BND_ICON_META_ELLIPSOID = BND_ICONID(23,18),
	BND_ICON_META_CAPSULE = BND_ICONID(24,18),

	BND_ICON_SURFACE_NCURVE = BND_ICONID(0,17),
	BND_ICON_SURFACE_NCIRCLE = BND_ICONID(1,17),
	BND_ICON_SURFACE_NSURFACE = BND_ICONID(2,17),
	BND_ICON_SURFACE_NCYLINDER = BND_ICONID(3,17),
	BND_ICON_SURFACE_NSPHERE = BND_ICONID(4,17),
	BND_ICON_SURFACE_NTORUS = BND_ICONID(5,17),
	BND_ICON_CURVE_BEZCURVE = BND_ICONID(9,17),
	BND_ICON_CURVE_BEZCIRCLE = BND_ICONID(10,17),
	BND_ICON_CURVE_NCURVE = BND_ICONID(11,17),
	BND_ICON_CURVE_NCIRCLE = BND_ICONID(12,17),
	BND_ICON_CURVE_PATH = BND_ICONID(13,17),
	BND_ICON_COLOR_RED = BND_ICONID(19,17),
	BND_ICON_COLOR_GREEN = BND_ICONID(20,17),
	BND_ICON_COLOR_BLUE = BND_ICONID(21,17),

	BND_ICON_FORCE_FORCE = BND_ICONID(0,16),
	BND_ICON_FORCE_WIND = BND_ICONID(1,16),
	BND_ICON_FORCE_VORTEX = BND_ICONID(2,16),
	BND_ICON_FORCE_MAGNETIC = BND_ICONID(3,16),
	BND_ICON_FORCE_HARMONIC = BND_ICONID(4,16),
	BND_ICON_FORCE_CHARGE = BND_ICONID(5,16),
	BND_ICON_FORCE_LENNARDJONES = BND_ICONID(6,16),
	BND_ICON_FORCE_TEXTURE = BND_ICONID(7,16),
	BND_ICON_FORCE_CURVE = BND_ICONID(8,16),
	BND_ICON_FORCE_BOID = BND_ICONID(9,16),
	BND_ICON_FORCE_TURBULENCE = BND_ICONID(10,16),
	BND_ICON_FORCE_DRAG = BND_ICONID(11,16),
	BND_ICON_FORCE_SMOKEFLOW = BND_ICONID(12,16),

	BND_ICON_MODIFIER = BND_ICONID(0,12),
	BND_ICON_MOD_WAVE = BND_ICONID(1,12),
	BND_ICON_MOD_BUILD = BND_ICONID(2,12),
	BND_ICON_MOD_DECIM = BND_ICONID(3,12),
	BND_ICON_MOD_MIRROR = BND_ICONID(4,12),
	BND_ICON_MOD_SOFT = BND_ICONID(5,12),
	BND_ICON_MOD_SUBSURF = BND_ICONID(6,12),
	BND_ICON_HOOK = BND_ICONID(7,12),
	BND_ICON_MOD_PHYSICS = BND_ICONID(8,12),
	BND_ICON_MOD_PARTICLES = BND_ICONID(9,12),
	BND_ICON_MOD_BOOLEAN = BND_ICONID(10,12),
	BND_ICON_MOD_EDGESPLIT = BND_ICONID(11,12),
	BND_ICON_MOD_ARRAY = BND_ICONID(12,12),
	BND_ICON_MOD_UVPROJECT = BND_ICONID(13,12),
	BND_ICON_MOD_DISPLACE = BND_ICONID(14,12),
	BND_ICON_MOD_CURVE = BND_ICONID(15,12),
	BND_ICON_MOD_LATTICE = BND_ICONID(16,12),
	BND_ICON_CONSTRAINT_DATA = BND_ICONID(17,12),
	BND_ICON_MOD_ARMATURE = BND_ICONID(18,12),
	BND_ICON_MOD_SHRINKWRAP = BND_ICONID(19,12),
	BND_ICON_MOD_CAST = BND_ICONID(20,12),
	BND_ICON_MOD_MESHDEFORM = BND_ICONID(21,12),
	BND_ICON_MOD_BEVEL = BND_ICONID(22,12),
	BND_ICON_MOD_SMOOTH = BND_ICONID(23,12),
	BND_ICON_MOD_SIMPLEDEFORM = BND_ICONID(24,12),
	BND_ICON_MOD_MASK = BND_ICONID(25,12),

	BND_ICON_MOD_CLOTH = BND_ICONID(0,11),
	BND_ICON_MOD_EXPLODE = BND_ICONID(1,11),
	BND_ICON_MOD_FLUIDSIM = BND_ICONID(2,11),
	BND_ICON_MOD_MULTIRES = BND_ICONID(3,11),
	BND_ICON_MOD_SMOKE = BND_ICONID(4,11),
	BND_ICON_MOD_SOLIDIFY = BND_ICONID(5,11),
	BND_ICON_MOD_SCREW = BND_ICONID(6,11),
	BND_ICON_MOD_VERTEX_WEIGHT = BND_ICONID(7,11),
	BND_ICON_MOD_DYNAMICPAINT = BND_ICONID(8,11),
	BND_ICON_MOD_REMESH = BND_ICONID(9,11),
	BND_ICON_MOD_OCEAN = BND_ICONID(10,11),
	BND_ICON_MOD_WARP = BND_ICONID(11,11),
	BND_ICON_MOD_SKIN = BND_ICONID(12,11),
	BND_ICON_MOD_TRIANGULATE = BND_ICONID(13,11),
	BND_ICON_MOD_WIREFRAME = BND_ICONID(14,11),

	BND_ICON_REC = BND_ICONID(0,10),
	BND_ICON_PLAY = BND_ICONID(1,10),
	BND_ICON_FF = BND_ICONID(2,10),
	BND_ICON_REW = BND_ICONID(3,10),
	BND_ICON_PAUSE = BND_ICONID(4,10),
	BND_ICON_PREV_KEYFRAME = BND_ICONID(5,10),
	BND_ICON_NEXT_KEYFRAME = BND_ICONID(6,10),
	BND_ICON_PLAY_AUDIO = BND_ICONID(7,10),
	BND_ICON_PLAY_REVERSE = BND_ICONID(8,10),
	BND_ICON_PREVIEW_RANGE = BND_ICONID(9,10),
	BND_ICON_ACTION_TWEAK = BND_ICONID(10,10),
	BND_ICON_PMARKER_ACT = BND_ICONID(11,10),
	BND_ICON_PMARKER_SEL = BND_ICONID(12,10),
	BND_ICON_PMARKER = BND_ICONID(13,10),
	BND_ICON_MARKER_HLT = BND_ICONID(14,10),
	BND_ICON_MARKER = BND_ICONID(15,10),
	BND_ICON_SPACE2 = BND_ICONID(16,10),
	BND_ICON_SPACE3 = BND_ICONID(17,10),
	BND_ICON_KEYINGSET = BND_ICONID(18,10),
	BND_ICON_KEY_DEHLT = BND_ICONID(19,10),
	BND_ICON_KEY_HLT = BND_ICONID(20,10),
	BND_ICON_MUTE_IPO_OFF = BND_ICONID(21,10),
	BND_ICON_MUTE_IPO_ON = BND_ICONID(22,10),
	BND_ICON_VISIBLE_IPO_OFF = BND_ICONID(23,10),
	BND_ICON_VISIBLE_IPO_ON = BND_ICONID(24,10),
	BND_ICON_DRIVER = BND_ICONID(25,10),

	BND_ICON_SOLO_OFF = BND_ICONID(0,9),
	BND_ICON_SOLO_ON = BND_ICONID(1,9),
	BND_ICON_FRAME_PREV = BND_ICONID(2,9),
	BND_ICON_FRAME_NEXT = BND_ICONID(3,9),
	BND_ICON_NLA_PUSHDOWN = BND_ICONID(4,9),
	BND_ICON_IPO_CONSTANT = BND_ICONID(5,9),
	BND_ICON_IPO_LINEAR = BND_ICONID(6,9),
	BND_ICON_IPO_BEZIER = BND_ICONID(7,9),
	BND_ICON_IPO_SINE = BND_ICONID(8,9),
	BND_ICON_IPO_QUAD = BND_ICONID(9,9),
	BND_ICON_IPO_CUBIC = BND_ICONID(10,9),
	BND_ICON_IPO_QUART = BND_ICONID(11,9),
	BND_ICON_IPO_QUINT = BND_ICONID(12,9),
	BND_ICON_IPO_EXPO = BND_ICONID(13,9),
	BND_ICON_IPO_CIRC = BND_ICONID(14,9),
	BND_ICON_IPO_BOUNCE = BND_ICONID(15,9),
	BND_ICON_IPO_ELASTIC = BND_ICONID(16,9),
	BND_ICON_IPO_BACK = BND_ICONID(17,9),
	BND_ICON_IPO_EASE_IN = BND_ICONID(18,9),
	BND_ICON_IPO_EASE_OUT = BND_ICONID(19,9),
	BND_ICON_IPO_EASE_IN_OUT = BND_ICONID(20,9),

	BND_ICON_VERTEXSEL = BND_ICONID(0,8),
	BND_ICON_EDGESEL = BND_ICONID(1,8),
	BND_ICON_FACESEL = BND_ICONID(2,8),
	BND_ICON_LOOPSEL = BND_ICONID(3,8),
	BND_ICON_ROTATE = BND_ICONID(5,8),
	BND_ICON_CURSOR = BND_ICONID(6,8),
	BND_ICON_ROTATECOLLECTION = BND_ICONID(7,8),
	BND_ICON_ROTATECENTER = BND_ICONID(8,8),
	BND_ICON_ROTACTIVE = BND_ICONID(9,8),
	BND_ICON_ALIGN = BND_ICONID(10,8),
	BND_ICON_SMOOTHCURVE = BND_ICONID(12,8),
	BND_ICON_SPHERECURVE = BND_ICONID(13,8),
	BND_ICON_ROOTCURVE = BND_ICONID(14,8),
	BND_ICON_SHARPCURVE = BND_ICONID(15,8),
	BND_ICON_LINCURVE = BND_ICONID(16,8),
	BND_ICON_NOCURVE = BND_ICONID(17,8),
	BND_ICON_RNDCURVE = BND_ICONID(18,8),
	BND_ICON_PROP_OFF = BND_ICONID(19,8),
	BND_ICON_PROP_ON = BND_ICONID(20,8),
	BND_ICON_PROP_CON = BND_ICONID(21,8),
	BND_ICON_SCULPT_DYNTOPO = BND_ICONID(22,8),
	BND_ICON_PARTICLE_POINT = BND_ICONID(23,8),
	BND_ICON_PARTICLE_TIP = BND_ICONID(24,8),
	BND_ICON_PARTICLE_PATH = BND_ICONID(25,8),

	BND_ICON_MAN_TRANS = BND_ICONID(0,7),
	BND_ICON_MAN_ROT = BND_ICONID(1,7),
	BND_ICON_MAN_SCALE = BND_ICONID(2,7),
	BND_ICON_MANIPUL = BND_ICONID(3,7),
	BND_ICON_SNAP_OFF = BND_ICONID(4,7),
	BND_ICON_SNAP_ON = BND_ICONID(5,7),
	BND_ICON_SNAP_NORMAL = BND_ICONID(6,7),
	BND_ICON_SNAP_INCREMENT = BND_ICONID(7,7),
	BND_ICON_SNAP_VERTEX = BND_ICONID(8,7),
	BND_ICON_SNAP_EDGE = BND_ICONID(9,7),
	BND_ICON_SNAP_FACE = BND_ICONID(10,7),
	BND_ICON_SNAP_VOLUME = BND_ICONID(11,7),
	BND_ICON_STICKY_UVS_LOC = BND_ICONID(13,7),
	BND_ICON_STICKY_UVS_DISABLE = BND_ICONID(14,7),
	BND_ICON_STICKY_UVS_VERT = BND_ICONID(15,7),
	BND_ICON_CLIPUV_DEHLT = BND_ICONID(16,7),
	BND_ICON_CLIPUV_HLT = BND_ICONID(17,7),
	BND_ICON_SNAP_PEEL_OBJECT = BND_ICONID(18,7),
	BND_ICON_GRID = BND_ICONID(19,7),

	BND_ICON_PASTEDOWN = BND_ICONID(0,6),
	BND_ICON_COPYDOWN = BND_ICONID(1,6),
	BND_ICON_PASTEFLIPUP = BND_ICONID(2,6),
	BND_ICON_PASTEFLIPDOWN = BND_ICONID(3,6),
	BND_ICON_SNAP_SURFACE = BND_ICONID(8,6),
	BND_ICON_AUTOMERGE_ON = BND_ICONID(9,6),
	BND_ICON_AUTOMERGE_OFF = BND_ICONID(10,6),
	BND_ICON_RETOPO = BND_ICONID(11,6),
	BND_ICON_UV_VERTEXSEL = BND_ICONID(12,6),
	BND_ICON_UV_EDGESEL = BND_ICONID(13,6),
	BND_ICON_UV_FACESEL = BND_ICONID(14,6),
	BND_ICON_UV_ISLANDSEL = BND_ICONID(15,6),
	BND_ICON_UV_SYNC_SELECT = BND_ICONID(16,6),

	BND_ICON_BBOX = BND_ICONID(0,5),
	BND_ICON_WIRE = BND_ICONID(1,5),
	BND_ICON_SOLID = BND_ICONID(2,5),
	BND_ICON_SMOOTH = BND_ICONID(3,5),
	BND_ICON_POTATO = BND_ICONID(4,5),
	BND_ICON_ORTHO = BND_ICONID(6,5),
	BND_ICON_LOCKVIEW_OFF = BND_ICONID(9,5),
	BND_ICON_LOCKVIEW_ON = BND_ICONID(10,5),
	BND_ICON_AXIS_SIDE = BND_ICONID(12,5),
	BND_ICON_AXIS_FRONT = BND_ICONID(13,5),
	BND_ICON_AXIS_TOP = BND_ICONID(14,5),
	BND_ICON_NDOF_DOM = BND_ICONID(15,5),
	BND_ICON_NDOF_TURN = BND_ICONID(16,5),
	BND_ICON_NDOF_FLY = BND_ICONID(17,5),
	BND_ICON_NDOF_TRANS = BND_ICONID(18,5),
	BND_ICON_LAYER_USED = BND_ICONID(19,5),
	BND_ICON_LAYER_ACTIVE = BND_ICONID(20,5),

	BND_ICON_SORTALPHA = BND_ICONID(0,3),
	BND_ICON_SORTBYEXT = BND_ICONID(1,3),
	BND_ICON_SORTTIME = BND_ICONID(2,3),
	BND_ICON_SORTSIZE = BND_ICONID(3,3),
	BND_ICON_LONGDISPLAY = BND_ICONID(4,3),
	BND_ICON_SHORTDISPLAY = BND_ICONID(5,3),
	BND_ICON_GHOST = BND_ICONID(6,3),
	BND_ICON_IMGDISPLAY = BND_ICONID(7,3),
	BND_ICON_SAVE_AS = BND_ICONID(8,3),
	BND_ICON_SAVE_COPY = BND_ICONID(9,3),
	BND_ICON_BOOKMARKS = BND_ICONID(10,3),
	BND_ICON_FONTPREVIEW = BND_ICONID(11,3),
	BND_ICON_FILTER = BND_ICONID(12,3),
	BND_ICON_NEWFOLDER = BND_ICONID(13,3),
	BND_ICON_OPEN_RECENT = BND_ICONID(14,3),
	BND_ICON_FILE_PARENT = BND_ICONID(15,3),
	BND_ICON_FILE_REFRESH = BND_ICONID(16,3),
	BND_ICON_FILE_FOLDER = BND_ICONID(17,3),
	BND_ICON_FILE_BLANK = BND_ICONID(18,3),
	BND_ICON_FILE_BLEND = BND_ICONID(19,3),
	BND_ICON_FILE_IMAGE = BND_ICONID(20,3),
	BND_ICON_FILE_MOVIE = BND_ICONID(21,3),
	BND_ICON_FILE_SCRIPT = BND_ICONID(22,3),
	BND_ICON_FILE_SOUND = BND_ICONID(23,3),
	BND_ICON_FILE_FONT = BND_ICONID(24,3),
	BND_ICON_FILE_TEXT = BND_ICONID(25,3),

	BND_ICON_RECOVER_AUTO = BND_ICONID(0,2),
	BND_ICON_SAVE_PREFS = BND_ICONID(1,2),
	BND_ICON_LINK_BLEND = BND_ICONID(2,2),
	BND_ICON_APPEND_BLEND = BND_ICONID(3,2),
	BND_ICON_IMPORT = BND_ICONID(4,2),
	BND_ICON_EXPORT = BND_ICONID(5,2),
	BND_ICON_EXTERNAL_DATA = BND_ICONID(6,2),
	BND_ICON_LOAD_FACTORY = BND_ICONID(7,2),
	BND_ICON_LOOP_BACK = BND_ICONID(13,2),
	BND_ICON_LOOP_FORWARDS = BND_ICONID(14,2),
	BND_ICON_BACK = BND_ICONID(15,2),
	BND_ICON_FORWARD = BND_ICONID(16,2),
	BND_ICON_FILE_BACKUP = BND_ICONID(24,2),
	BND_ICON_DISK_DRIVE = BND_ICONID(25,2),

	BND_ICON_MATPLANE = BND_ICONID(0,1),
	BND_ICON_MATSPHERE = BND_ICONID(1,1),
	BND_ICON_MATCUBE = BND_ICONID(2,1),
	BND_ICON_MONKEY = BND_ICONID(3,1),
	BND_ICON_HAIR = BND_ICONID(4,1),
	BND_ICON_ALIASED = BND_ICONID(5,1),
	BND_ICON_ANTIALIASED = BND_ICONID(6,1),
	BND_ICON_MAT_SPHERE_SKY = BND_ICONID(7,1),
	BND_ICON_WORDWRAP_OFF = BND_ICONID(12,1),
	BND_ICON_WORDWRAP_ON = BND_ICONID(13,1),
	BND_ICON_SYNTAX_OFF = BND_ICONID(14,1),
	BND_ICON_SYNTAX_ON = BND_ICONID(15,1),
	BND_ICON_LINENUMBERS_OFF = BND_ICONID(16,1),
	BND_ICON_LINENUMBERS_ON = BND_ICONID(17,1),
	BND_ICON_SCRIPTPLUGINS = BND_ICONID(18,1),

	BND_ICON_SEQ_SEQUENCER = BND_ICONID(0,0),
	BND_ICON_SEQ_PREVIEW = BND_ICONID(1,0),
	BND_ICON_SEQ_LUMA_WAVEFORM = BND_ICONID(2,0),
	BND_ICON_SEQ_CHROMA_SCOPE = BND_ICONID(3,0),
	BND_ICON_SEQ_HISTOGRAM = BND_ICONID(4,0),
	BND_ICON_SEQ_SPLITVIEW = BND_ICONID(5,0),
	BND_ICON_IMAGE_RGB = BND_ICONID(9,0),
	BND_ICON_IMAGE_RGB_ALPHA = BND_ICONID(10,0),
	BND_ICON_IMAGE_ALPHA = BND_ICONID(11,0),
	BND_ICON_IMAGE_ZDEPTH = BND_ICONID(12,0),
	BND_ICON_IMAGEFILE = BND_ICONID(13,0),
} BNDicon;

////////////////////////////////////////////////////////////////////////////////

// set the current theme all widgets will be drawn with.
// the default Blender 2.6 theme is set by default.
BND_EXPORT void bndSetTheme(BNDtheme theme);

// Returns the currently set theme
BND_EXPORT const BNDtheme *bndGetTheme();

// designates an image handle as returned by nvgCreateImage*() as the themes'
// icon sheet. The icon sheet format must be compatible to Blender 2.6's icon 
// sheet; the order of icons does not matter.
// A valid icon sheet is e.g. shown at
// http://wiki.blender.org/index.php/Dev:2.5/Doc/How_to/Add_an_icon
BND_EXPORT void bndSetIconImage(int image);

// designates an image handle as returned by nvgCreateFont*() as the themes'
// UI font. Blender's original UI font Droid Sans is perfectly suited and
// available here:
// https://svn.blender.org/svnroot/bf-blender/trunk/blender/release/datafiles/fonts/
BND_EXPORT void bndSetFont(int font);

////////////////////////////////////////////////////////////////////////////////

// High Level Functions
// --------------------
// Use these functions to draw themed widgets with your NVGcontext.

// Draw a label with its lower left origin at (x,y) and size of (w,h).
// if iconid >= 0, an icon will be added to the widget
// if label is not NULL, a label will be added to the widget
// widget looks best when height is BND_WIDGET_HEIGHT
BND_EXPORT void bndLabel(NVGcontext *ctx,
    float x, float y, float w, float h, int iconid, const char *label);

// Draw a tool button  with its lower left origin at (x,y) and size of (w,h),
// where flags is one or multiple flags from BNDcornerFlags and state denotes
// the widgets current UI state.
// if iconid >= 0, an icon will be added to the widget
// if label is not NULL, a label will be added to the widget
// widget looks best when height is BND_WIDGET_HEIGHT
BND_EXPORT void bndToolButton(NVGcontext *ctx,
    float x, float y, float w, float h, int flags, BNDwidgetState state, 
    int iconid, const char *label);

// Draw a radio button with its lower left origin at (x,y) and size of (w,h),
// where flags is one or multiple flags from BNDcornerFlags and state denotes
// the widgets current UI state.
// if iconid >= 0, an icon will be added to the widget
// if label is not NULL, a label will be added to the widget
// widget looks best when height is BND_WIDGET_HEIGHT
BND_EXPORT void bndRadioButton(NVGcontext *ctx,
    float x, float y, float w, float h, int flags, BNDwidgetState state, 
    int iconid, const char *label);
    

// Calculate the corresponding text position for given coordinates px/py
// in a text field.
// See bndTextField for more info.
BND_EXPORT int bndTextFieldTextPosition(NVGcontext *ctx, float x, float y, float w, float h,
    int iconid, const char *text, int px, int py);

// Draw a text field with its lower left origin at (x,y) and size of (w,h),
// where flags is one or multiple flags from BNDcornerFlags and state denotes
// the widgets current UI state.
// if iconid >= 0, an icon will be added to the widget
// if text is not NULL, text will be printed to the widget
// cbegin must be >= 0 and <= strlen(text) and denotes the beginning of the caret
// cend must be >= cbegin and <= strlen(text) and denotes the end of the caret
// if cend < cbegin, then no caret will be drawn
// widget looks best when height is BND_WIDGET_HEIGHT
BND_EXPORT void bndTextField(NVGcontext *ctx,
    float x, float y, float w, float h, int flags, BNDwidgetState state, 
    int iconid, const char *text, int cbegin, int cend);
    
// Draw an option button with its lower left origin at (x,y) and size of (w,h),
// where flags is one or multiple flags from BNDcornerFlags and state denotes
// the widgets current UI state.
// if label is not NULL, a label will be added to the widget
// widget looks best when height is BND_WIDGET_HEIGHT
BND_EXPORT void bndOptionButton(NVGcontext *ctx,
    float x, float y, float w, float h, BNDwidgetState state, 
    const char *label);
    
// Draw a choice button with its lower left origin at (x,y) and size of (w,h),
// where flags is one or multiple flags from BNDcornerFlags and state denotes
// the widgets current UI state.
// if iconid >= 0, an icon will be added to the widget
// if label is not NULL, a label will be added to the widget
// widget looks best when height is BND_WIDGET_HEIGHT
BND_EXPORT void bndChoiceButton(NVGcontext *ctx,
    float x, float y, float w, float h, int flags, BNDwidgetState state, 
    int iconid, const char *label);
    
// Draw a color button  with its lower left origin at (x,y) and size of (w,h),
// where flags is one or multiple flags from BNDcornerFlags and state denotes
// the widgets current UI state.
// widget looks best when height is BND_WIDGET_HEIGHT
BND_EXPORT void bndColorButton(NVGcontext *ctx,
    float x, float y, float w, float h, int flags, NVGcolor color);

// Draw a number field with its lower left origin at (x,y) and size of (w,h),
// where flags is one or multiple flags from BNDcornerFlags and state denotes
// the widgets current UI state.
// if label is not NULL, a label will be added to the widget
// if value is not NULL, a value will be added to the widget, along with
// a ":" separator
// widget looks best when height is BND_WIDGET_HEIGHT
BND_EXPORT void bndNumberField(NVGcontext *ctx,
    float x, float y, float w, float h, int flags, BNDwidgetState state, 
    const char *label, const char *value);    

// Draw slider control with its lower left origin at (x,y) and size of (w,h),
// where flags is one or multiple flags from BNDcornerFlags and state denotes
// the widgets current UI state.
// progress must be in the range 0..1 and controls the size of the slider bar
// if label is not NULL, a label will be added to the widget
// if value is not NULL, a value will be added to the widget, along with
// a ":" separator
// widget looks best when height is BND_WIDGET_HEIGHT
BND_EXPORT void bndSlider(NVGcontext *ctx,
    float x, float y, float w, float h, int flags, BNDwidgetState state, 
    float progress, const char *label, const char *value);

// Draw scrollbar with its lower left origin at (x,y) and size of (w,h),
// where state denotes the widgets current UI state.
// offset is in the range 0..1 and controls the position of the scroll handle
// size is in the range 0..1 and controls the size of the scroll handle
// horizontal widget looks best when height is BND_SCROLLBAR_HEIGHT,
// vertical looks best when width is BND_SCROLLBAR_WIDTH
BND_EXPORT void bndScrollBar(NVGcontext *ctx,
    float x, float y, float w, float h, BNDwidgetState state, 
    float offset, float size);
    
// Draw a menu background with its lower left origin at (x,y) and size of (w,h),
// where flags is one or multiple flags from BNDcornerFlags.
BND_EXPORT void bndMenuBackground(NVGcontext *ctx,
    float x, float y, float w, float h, int flags);

// Draw a menu label with its lower left origin at (x,y) and size of (w,h).
// if iconid >= 0, an icon will be added to the widget
// if label is not NULL, a label will be added to the widget
// widget looks best when height is BND_WIDGET_HEIGHT
BND_EXPORT void bndMenuLabel(NVGcontext *ctx,
    float x, float y, float w, float h, int iconid, const char *label);

// Draw a menu item with its lower left origin at (x,y) and size of (w,h),
// where state denotes the widgets current UI state.
// if iconid >= 0, an icon will be added to the widget
// if label is not NULL, a label will be added to the widget
// widget looks best when height is BND_WIDGET_HEIGHT
BND_EXPORT void bndMenuItem(NVGcontext *ctx,
    float x, float y, float w, float h, BNDwidgetState state, 
    int iconid, const char *label);

// Draw a tooltip background with its lower left origin at (x,y) and size of (w,h)
BND_EXPORT void bndTooltipBackground(NVGcontext *ctx, float x, float y, float w, float h);

// Draw a node port at the given position filled with the given color
BND_EXPORT void bndNodePort(NVGcontext *ctx, float x, float y, BNDwidgetState state,
    NVGcolor color);

// Draw a node wire originating at (x0,y0) and floating to (x1,y1), with 
// a colored gradient based on the states state0 and state1:
// BND_DEFAULT: default wire color
// BND_HOVER: selected wire color
// BND_ACTIVE: dragged wire color
BND_EXPORT void bndNodeWire(NVGcontext *ctx, float x0, float y0, float x1, float y1,
    BNDwidgetState state0, BNDwidgetState state1);

// Draw a node wire originating at (x0,y0) and floating to (x1,y1), with
// a colored gradient based on the two colors color0 and color1
BND_EXPORT void bndColoredNodeWire(NVGcontext *ctx, float x0, float y0, float x1, float y1,
    NVGcolor color0, NVGcolor color1);

// Draw a node background with its upper left origin at (x,y) and size of (w,h)
// where titleColor provides the base color for the title bar
BND_EXPORT void bndNodeBackground(NVGcontext *ctx, float x, float y, float w, float h,
    BNDwidgetState state, int iconid, const char *label, NVGcolor titleColor);

// Draw a window with the upper right and lower left splitter widgets into 
// the rectangle at origin (x,y) and size (w, h)
BND_EXPORT void bndSplitterWidgets(NVGcontext *ctx, float x, float y, float w, float h);

// Draw the join area overlay stencil into the rectangle
// at origin (x,y) and size (w,h)
// vertical is 0 or 1 and designates the arrow orientation, 
// mirror is 0 or 1 and flips the arrow side
BND_EXPORT void bndJoinAreaOverlay(NVGcontext *ctx, float x, float y, float w, float h,
    int vertical, int mirror);

////////////////////////////////////////////////////////////////////////////////
        
// Estimator Functions
// -------------------
// Use these functions to estimate sizes for widgets with your NVGcontext.

// returns the ideal width for a label with given icon and text
BND_EXPORT float bndLabelWidth(NVGcontext *ctx, int iconid, const char *label);

// returns the height for a label with given icon, text and width; this
// function is primarily useful in conjunction with multiline labels and textboxes
BND_EXPORT float bndLabelHeight(NVGcontext *ctx, int iconid, const char *label,
    float width);

////////////////////////////////////////////////////////////////////////////////

// Low Level Functions
// -------------------
// these are part of the implementation detail and can be used to theme
// new kinds of controls in a similar fashion.

// make color transparent using the default alpha value
BND_EXPORT NVGcolor bndTransparent(NVGcolor color);

// offset a color by a given integer delta in the range -100 to 100
BND_EXPORT NVGcolor bndOffsetColor(NVGcolor color, int delta);

// assigns radius r to the four entries of array radiuses depending on whether
// the corner is marked as sharp or not; see BNDcornerFlags for possible
// flag values.
BND_EXPORT void bndSelectCorners(float *radiuses, float r, int flags);

// computes the upper and lower gradient colors for the inner box from a widget
// theme and the widgets state. If flipActive is set and the state is 
// BND_ACTIVE, the upper and lower colors will be swapped.
BND_EXPORT void bndInnerColors(NVGcolor *shade_top, NVGcolor *shade_down,
    const BNDwidgetTheme *theme, BNDwidgetState state, int flipActive);

// computes the text color for a widget label from a widget theme and the 
// widgets state.
BND_EXPORT NVGcolor bndTextColor(const BNDwidgetTheme *theme, BNDwidgetState state);

// computes the bounds of the scrollbar handle from the scrollbar size
// and the handles offset and size.
// offset is in the range 0..1 and defines the position of the scroll handle
// size is in the range 0..1 and defines the size of the scroll handle
BND_EXPORT void bndScrollHandleRect(float *x, float *y, float *w, float *h,
    float offset, float size);

// Add a rounded box path at position (x,y) with size (w,h) and a separate 
// radius for each corner listed in clockwise order, so that cr0 = top left,
// cr1 = top right, cr2 = bottom right, cr3 = bottom left;
// this is a low level drawing function: the path must be stroked or filled
// to become visible.
BND_EXPORT void bndRoundedBox(NVGcontext *ctx, float x, float y, float w, float h,
    float cr0, float cr1, float cr2, float cr3);

// Draw a flat panel without any decorations at position (x,y) with size (w,h)
// and fills it with backgroundColor
BND_EXPORT void bndBackground(NVGcontext *ctx, float x, float y, float w, float h);

// Draw a beveled border at position (x,y) with size (w,h) shaded with
// lighter and darker versions of backgroundColor
BND_EXPORT void bndBevel(NVGcontext *ctx, float x, float y, float w, float h);

// Draw a lower inset for a rounded box at position (x,y) with size (w,h)
// that gives the impression the surface has been pushed in.
// cr2 and cr3 contain the radiuses of the bottom right and bottom left
// corners of the rounded box.
BND_EXPORT void bndBevelInset(NVGcontext *ctx, float x, float y, float w, float h,
    float cr2, float cr3);
    
// Draw an icon with (x,y) as its upper left coordinate; the iconid selects
// the icon from the sheet; use the BND_ICONID macro to build icon IDs.
BND_EXPORT void bndIcon(NVGcontext *ctx, float x, float y, int iconid);

// Draw a drop shadow around the rounded box at (x,y) with size (w,h) and
// radius r, with feather as its maximum range in pixels.
// No shadow will be painted inside the rounded box.
BND_EXPORT void bndDropShadow(NVGcontext *ctx, float x, float y, float w, float h,
    float r, float feather, float alpha);

// Draw the inner part of a widget box, with a gradient from shade_top to 
// shade_down. If h>w, the gradient will be horizontal instead of
// vertical.
BND_EXPORT void bndInnerBox(NVGcontext *ctx, float x, float y, float w, float h,
    float cr0, float cr1, float cr2, float cr3,
    NVGcolor shade_top, NVGcolor shade_down);

// Draw the outline part of a widget box with the given color
BND_EXPORT void bndOutlineBox(NVGcontext *ctx, float x, float y, float w, float h,
    float cr0, float cr1, float cr2, float cr3, NVGcolor color);

// Draw an optional icon specified by <iconid> and an optional label with
// given alignment (BNDtextAlignment), fontsize and color within a widget box.
// if iconid is >= 0, an icon will be drawn and the labels remaining space
// will be adjusted.
// if label is not NULL, it will be drawn with the specified alignment, fontsize
// and color.
// if value is not NULL, label and value will be drawn with a ":" separator
// inbetween.
BND_EXPORT void bndIconLabelValue(NVGcontext *ctx, float x, float y, float w, float h,
    int iconid, NVGcolor color, int align, float fontsize, const char *label, 
    const char *value);

// Draw an optional icon specified by <iconid> and an optional label with
// given alignment (BNDtextAlignment), fontsize and color within a node title bar
// if iconid is >= 0, an icon will be drawn
// if label is not NULL, it will be drawn with the specified alignment, fontsize
// and color.
BND_EXPORT void bndNodeIconLabel(NVGcontext *ctx, float x, float y, float w, float h,
    int iconid, NVGcolor color, NVGcolor shadowColor, int align, 
    float fontsize, const char *label);

// Calculate the corresponding text position for given coordinates px/py
// in an iconLabel.
// See bndIconLabelCaret for more info.
BND_EXPORT int bndIconLabelTextPosition(NVGcontext *ctx, float x, float y, float w, float h,
    int iconid, float fontsize, const char *label, int px, int py);
    
// Draw an optional icon specified by <iconid>, an optional label and 
// a caret with given fontsize and color within a widget box.
// if iconid is >= 0, an icon will be drawn and the labels remaining space
// will be adjusted.
// if label is not NULL, it will be drawn with the specified alignment, fontsize
// and color.
// cbegin must be >= 0 and <= strlen(text) and denotes the beginning of the caret
// cend must be >= cbegin and <= strlen(text) and denotes the end of the caret
// if cend < cbegin, then no caret will be drawn
BND_EXPORT void bndIconLabelCaret(NVGcontext *ctx, float x, float y, float w, float h,
    int iconid, NVGcolor color, float fontsize, const char *label, 
    NVGcolor caretcolor, int cbegin, int cend);
    
// Draw a checkmark for an option box with the given upper left coordinates
// (ox,oy) with the specified color.
BND_EXPORT void bndCheck(NVGcontext *ctx, float ox, float oy, NVGcolor color);

// Draw a horizontal arrow for a number field with its center at (x,y) and
// size s; if s is negative, the arrow points to the left.
BND_EXPORT void bndArrow(NVGcontext *ctx, float x, float y, float s, NVGcolor color);

// Draw an up/down arrow for a choice box with its center at (x,y) and size s
BND_EXPORT void bndUpDownArrow(NVGcontext *ctx, float x, float y, float s, NVGcolor color);

// Draw a node down-arrow with its tip at (x,y) and size s
BND_EXPORT void bndNodeArrowDown(NVGcontext *ctx, float x, float y, float s, NVGcolor color);

// return the color of a node wire based on state
// BND_HOVER indicates selected state, 
// BND_ACTIVE indicates dragged state
BND_EXPORT NVGcolor bndNodeWireColor(const BNDnodeTheme *theme, BNDwidgetState state);

#ifdef __cplusplus
};
#endif

#endif // BLENDISH_H
