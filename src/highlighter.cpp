#include "highlighter.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "colors.h"

// Define this macro to output messages when non closing quotes
#define NON_CLOSING_QUOTE_MSG

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
    rule.format.setFontUnderline(true);
    highlightingRules.append(rule);
    rule.format.setFontUnderline(false);

    // Comments //
    rule.pattern = QRegularExpression("\\(\\*.*\\*\\)");
    rule.format.setForeground(COMMENT);
    highlightingRules.append(rule);

    // Top Level //
    rule.pattern = QRegularExpression("OCaml version.*\\b|#|;;|val|:");
    rule.format.setForeground(TOPLEVEL);
    highlightingRules.append(rule);

    // Strings //
    stringFormat = QTextCharFormat();
    stringFormat.setForeground(STRING);
}

// For each rule of highlightingRules find and highlight matches
void Highlighter::highlightBlock(const QString &text)
{
    // Global //
    foreach (const HighlightingRule &rule, highlightingRules)
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Special //
    // Strings
    // For each opening double quote, find the closing one and format
    for (int i = 0; i < text.length() - 2; ++i)
    {
        // Char with single quote
        if (text[i] == '\'')
        {
            const int START = i;
            int end;

            // Special char
            if (text[i + 1] == '\\')
                end = i + 3;
            else
                end = i + 2;

            // Non closing quote
            if (end >= text.length() || text[end] != '\'')
            {
#ifdef NON_CLOSING_QUOTE_MSG
                MainWindow::instance->ui->statusbar->showMessage("Non closing single quote");
#endif
                break;
            }

            // Format
            setFormat(START, end - START + 1, stringFormat);

            i = end;
        }

        // We find the opening double quote
        else if (text[i] == '"' && (i == 0 || text[i - 1] != '\''))
        {
            const int START = i;
            int end = -1;

            for (++i; i < text.length(); ++i)
                // Closing one found
                if (text[i] == '"' && text[i - 1] != '\\')
                {
                    end = i;
                    break;
                }

            // Non closing quote
            if (end == -1)
            {
#ifdef NON_CLOSING_QUOTE_MSG
                MainWindow::instance->ui->statusbar->showMessage("Non closing double quote");
#endif
                break;
            }

            // Format
            setFormat(START, end - START + 1, stringFormat);
        }
   }
}
