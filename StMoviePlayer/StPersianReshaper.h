#ifndef __StPersianReshaper_h_
#define __StPersianReshaper_h_

#include <StStrings/StString.h>
#include <vector>
#include <map>
#include <algorithm>

struct StArabicGlyph {
    uint32_t isolated;
    uint32_t finalForm;
    uint32_t initialForm;
    uint32_t medialForm;
    bool connectsToNext;
};

class StPersianReshaper {
public:
    static bool isPersianChar(uint32_t ch) {
        return (ch >= 0x0600 && ch <= 0x06FF) || (ch >= 0x0750 && ch <= 0x077F) || (ch >= 0xFB50 && ch <= 0xFEFF);
    }

    static StString reshapeAndRTL(const StString& theInput) {
        static const std::map<uint32_t, StArabicGlyph> aMap = {
            {0x0622, {0xFE81, 0xFE82, 0xFE81, 0xFE82, false}}, // آ
            {0x0627, {0xFE8D, 0xFE8E, 0xFE8D, 0xFE8E, false}}, // ا
            {0x0628, {0xFE8F, 0xFE90, 0xFE91, 0xFE92, true}},  // ب
            {0x067E, {0xFB56, 0xFB57, 0xFB58, 0xFB59, true}},  // پ
            {0x062A, {0xFE95, 0xFE96, 0xFE97, 0xFE98, true}},  // ت
            {0x062B, {0xFE99, 0xFE9A, 0xFE9B, 0xFE9C, true}},  // ث
            {0x062C, {0xFE9D, 0xFE9E, 0xFE9F, 0xFEA0, true}},  // ج
            {0x0686, {0xFB7A, 0xFB7B, 0xFB7C, 0xFB7D, true}},  // چ
            {0x062D, {0xFEA1, 0xFEA2, 0xFEA3, 0xFEA4, true}},  // ح
            {0x062E, {0xFEA5, 0xFEA6, 0xFEA7, 0xFEA8, true}},  // خ
            {0x062F, {0xFEA9, 0xFEAA, 0xFEA9, 0xFEAA, false}}, // د
            {0x0630, {0xFEAB, 0xFEAC, 0xFEAB, 0xFEAC, false}}, // ذ
            {0x0631, {0xFEAD, 0xFEAE, 0xFEAD, 0xFEAE, false}}, // ر
            {0x0632, {0xFEAF, 0xFEB0, 0xFEAF, 0xFEB0, false}}, // ز
            {0x0698, {0xFB8A, 0xFB8B, 0xFB8A, 0xFB8B, false}}, // ژ
            {0x0633, {0xFEB1, 0xFEB2, 0xFEB3, 0xFEB4, true}},  // س
            {0x0634, {0xFEB5, 0xFEB6, 0xFEB7, 0xFEB8, true}},  // ش
            {0x0635, {0xFEB9, 0xFEBA, 0xFEBB, 0xFEBC, true}},  // ص
            {0x0636, {0xFEBD, 0xFEBE, 0xFEBF, 0xFEC0, true}},  // ض
            {0x0637, {0xFEC1, 0xFEC2, 0xFEC3, 0xFEC4, true}},  // ط
            {0x0638, {0xFEC5, 0xFEC6, 0xFEC7, 0xFEC8, true}},  // ظ
            {0x0639, {0xFEC9, 0xFECA, 0xFECB, 0xFECC, true}},  // ع
            {0x063A, {0xFECD, 0xFECE, 0xFECF, 0xFED0, true}},  // غ
            {0x0641, {0xFED1, 0xFED2, 0xFED3, 0xFED4, true}},  // ف
            {0x0642, {0xFED5, 0xFED6, 0xFED7, 0xFED8, true}},  // ق
            {0x06A9, {0xFB8E, 0xFB8F, 0xFB90, 0xFB91, true}},  // ک
            {0x0643, {0xFB8E, 0xFB8F, 0xFB90, 0xFB91, true}},  // ك عربی -> ک
            {0x06AF, {0xFB92, 0xFB93, 0xFB94, 0xFB95, true}},  // گ
            {0x0644, {0xFEDD, 0xFEDE, 0xFEDF, 0xFEE0, true}},  // ل
            {0x0645, {0xFEE1, 0xFEE2, 0xFEE3, 0xFEE4, true}},  // م
            {0x0646, {0xFEE5, 0xFEE6, 0xFEE7, 0xFEE8, true}},  // ن
            {0x0648, {0xFEED, 0xFEEE, 0xFEED, 0xFEEE, false}}, // و
            {0x0647, {0xFEE9, 0xFEEA, 0xFEEB, 0xFEEC, true}},  // ه
            {0x06CC, {0xFBFC, 0xFBFD, 0xFBFE, 0xFBFF, true}},  // ی
            {0x064A, {0xFBFC, 0xFBFD, 0xFBFE, 0xFBFF, true}},  // ي عربی -> ی
            {0x0626, {0xFE89, 0xFE8A, 0xFE8B, 0xFE8C, true}}   // ئ
        };

        std::vector<uint32_t> aChars;
        for (StUtf8Iter anIter = theInput.iterator(); *anIter != 0; ++anIter) {
            aChars.push_back(*anIter);
        }
        if (aChars.empty()) return theInput;

        bool hasPersian = false;
        for (uint32_t ch : aChars) {
            if (isPersianChar(ch)) { hasPersian = true; break; }
        }
        if (!hasPersian) return theInput;

        // ۱. اتصال حروف (Reshaping)
        std::vector<uint32_t> aReshaped;
        for (size_t i = 0; i < aChars.size(); ++i) {
            uint32_t curr = aChars[i];
            auto itCurr = aMap.find(curr);
            if (itCurr == aMap.end()) {
                aReshaped.push_back(curr);
                continue;
            }

            // ترکیب لا (ل + ا)
            if (curr == 0x0644 && i + 1 < aChars.size() && (aChars[i + 1] == 0x0627 || aChars[i + 1] == 0x0622)) {
                bool prevConnects = (i > 0 && aMap.count(aChars[i - 1]) && aMap.at(aChars[i - 1]).connectsToNext);
                if (aChars[i + 1] == 0x0622) aReshaped.push_back(prevConnects ? 0xFEF6 : 0xFEF5);
                else aReshaped.push_back(prevConnects ? 0xFEFC : 0xFEFB);
                ++i;
                continue;
            }

            bool prevConnects = (i > 0 && aMap.count(aChars[i - 1]) && aMap.at(aChars[i - 1]).connectsToNext);
            bool nextConnects = (i + 1 < aChars.size() && aMap.count(aChars[i + 1]));

            if (prevConnects && nextConnects)      aReshaped.push_back(itCurr->second.medialForm);
            else if (prevConnects)                 aReshaped.push_back(itCurr->second.finalForm);
            else if (nextConnects)                 aReshaped.push_back(itCurr->second.initialForm);
            else                                   aReshaped.push_back(itCurr->second.isolated);
        }

        // ۲. معکوس‌سازی خط به خط (RTL) با حفظ ترتیب اعداد و کلمات انگلیسی
        std::reverse(aReshaped.begin(), aReshaped.end());
        size_t i = 0;
        while (i < aReshaped.size()) {
            if ((aReshaped[i] >= 'A' && aReshaped[i] <= 'Z') ||
                (aReshaped[i] >= 'a' && aReshaped[i] <= 'z') ||
                (aReshaped[i] >= '0' && aReshaped[i] <= '9')) {
                size_t start = i;
                while (i < aReshaped.size() &&
                      ((aReshaped[i] >= 'A' && aReshaped[i] <= 'Z') ||
                       (aReshaped[i] >= 'a' && aReshaped[i] <= 'z') ||
                       (aReshaped[i] >= '0' && aReshaped[i] <= '9') ||
                       aReshaped[i] == '.' || aReshaped[i] == '-')) {
                    ++i;
                }
                std::reverse(aReshaped.begin() + start, aReshaped.begin() + i);
            } else {
                ++i;
            }
        }

        StString aResult;
        for (uint32_t cp : aReshaped) {
            aResult += stUtf32_t(cp);
        }
        return aResult;
    }
};

#endif
