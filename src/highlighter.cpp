#include "highlighter.h"

#include "colors.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    using namespace color;

    // Higher rule added in code = Lower priority
    // The rule used for each pattern
    HighlightingRule rule;
    rule.format = QTextCharFormat();

    // Identifier //
    rule.pattern = QRegularExpression("[\\w]+");
    rule.format.setForeground(IDENTIFIER);
    highlightingRules.append(rule);

    // Operator //
    rule.pattern = QRegularExpression("([\\+\\-\\*/][\\.]?)|[<>=!]|(::)|mod");
    rule.format.setForeground(OPERATOR);
    highlightingRules.append(rule);

    // Numeric //
    rule.pattern = QRegularExpression("[\\d]+[.]?[\\d]*");
    rule.format.setForeground(NUMERIC);
    highlightingRules.append(rule);

    // Keywords //
    rule.pattern = QRegularExpression("\\b("
                                      // Variable
                                      "let|function|rec|\\->|and|in|"
                                      // Control flow
                                      "if|else|match|;|"
                                      ")\\b");
    rule.format.setForeground(KEYWORD);
    highlightingRules.append(rule);

    // Comments //
    rule.pattern = QRegularExpression("\\(\\*.*\\*\\)");
    rule.format.setForeground(COMMENT);
    highlightingRules.append(rule);

    // Top Level //
    rule.pattern = QRegularExpression("OCaml version.*\\b|#|;;|val|:");
    rule.format.setForeground(TOPLEVEL);
    highlightingRules.append(rule);


    // WIP :
    // Problem : Matches words between two strings
    // TODO : Don't make global match but test single double quotes chars
    // //// Special ////
    // // Strings //
    // stringRule.pattern = QRegularExpression("\"(.(?!\"))*\""); //"(?<!\\\\)\"(.(?!\"))*(?<!\\\\)\"");
    // stringRule.format = QTextCharFormat();
    // stringRule.format.setForeground(STRING);
    // highlightingRules.append(rule);
}

// For each rule of highlightingRules find and highlight matches
void Highlighter::highlightBlock(const QString &text)
{
    // Global //
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // WIP (See WIP in the constructor
//    // Special //
//    // Strings
//    {
//        // The opening quote
//        int currentQuoteIndex = -1;

//        QRegularExpressionMatchIterator matchIterator = stringRule.pattern.globalMatch(text); //, 0, QRegularExpression::MatchType::PartialPreferFirstMatch);
//        while (matchIterator.hasNext()) {
//            QRegularExpressionMatch match = matchIterator.next();

////            // Only for new double quotes
////            if (currentQuoteIndex >= match.capturedStart())
////                continue;

////            currentQuoteIndex = match.capturedEnd();
//            setFormat(match.capturedStart(), match.capturedLength(), stringRule.format);

////            // Pass one match
////            if (matchIterator.hasNext())
////                matchIterator.next();
//        }
//    }
}
