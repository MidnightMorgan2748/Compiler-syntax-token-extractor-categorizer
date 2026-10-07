#include "JsonExporter.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

JsonExporter::JsonExporter() {}

std::string JsonExporter::escapeJsonString(const std::string& input) {
    std::ostringstream ss;
    for (char c : input) {
        switch (c) {
            case '"':  ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b";  break;
            case '\f': ss << "\\f";  break;
            case '\n': ss << "\\n";  break;
            case '\r': ss << "\\r";  break;
            case '\t': ss << "\\t";  break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    ss << c;
                }
                break;
        }
    }
    return ss.str();
}

bool JsonExporter::exportAnalysis(const std::string& outputPath,
                                  const std::string& sourceName,
                                  const std::vector<std::string>& rawTokens,
                                  const std::vector<Token>& detailedTokens,
                                  const ExtractionResult& extractionResult,
                                  const CategorizationReport& categorizationReport,
                                  const std::vector<BenchmarkRow>& benchmarkRows) {
    std::ofstream out(outputPath);
    if (!out.is_open()) {
        std::cerr << "Error: Could not open JSON output file: " << outputPath << "\n";
        return false;
    }

    out << "{\n";
    out << "  \"projectName\": \"Compiler Syntax Token Unique Extractor & Categorizer\",\n";
    out << "  \"group\": \"Group 5\",\n";
    out << "  \"sourceName\": \"" << escapeJsonString(sourceName) << "\",\n";
    
    // 1. Summary Metrics
    out << "  \"metrics\": {\n";
    out << "    \"totalTokensIngested\": " << extractionResult.totalTokensIngested << ",\n";
    out << "    \"totalKeywordOccurrences\": " << extractionResult.totalKeywordOccurrences << ",\n";
    out << "    \"uniqueKeywordsCount\": " << extractionResult.uniqueKeywordCount << ",\n";
    out << "    \"duplicateRejectionsCount\": " << extractionResult.duplicateRejectionsCount << ",\n";
    out << "    \"keywordDensityPercentage\": " << std::fixed << std::setprecision(2) << extractionResult.keywordDensityPercentage << ",\n";
    out << "    \"uniquenessRatio\": " << extractionResult.uniquenessRatio << ",\n";
    out << "    \"duplicateSuppressionRatio\": " << extractionResult.duplicateSuppressionRatio << "\n";
    out << "  },\n";

    // 2. Lexicographically Sorted Unique Keywords (std::set)
    out << "  \"uniqueKeywordsLexicographical\": [\n";
    size_t kwIdx = 0;
    for (const auto& kw : extractionResult.uniqueKeywords) {
        out << "    \"" << escapeJsonString(kw) << "\"" << (++kwIdx < extractionResult.uniqueKeywords.size() ? "," : "") << "\n";
    }
    out << "  ],\n";

    // 3. Detailed Keyword Frequency Table
    out << "  \"keywordFrequencies\": [\n";
    for (size_t i = 0; i < extractionResult.detailedMetrics.size(); ++i) {
        const auto& m = extractionResult.detailedMetrics[i];
        out << "    {\n";
        out << "      \"keyword\": \"" << escapeJsonString(m.keyword) << "\",\n";
        out << "      \"category\": \"" << escapeJsonString(keywordCategoryToString(m.category)) << "\",\n";
        out << "      \"frequency\": " << m.originalFrequency << ",\n";
        out << "      \"relativeFrequencyPercentage\": " << std::fixed << std::setprecision(2) << m.relativeFrequency << ",\n";
        out << "      \"streamPercentage\": " << m.streamPercentage << "\n";
        out << "    }" << (i + 1 < extractionResult.detailedMetrics.size() ? "," : "") << "\n";
    }
    out << "  ],\n";

    // 4. Categorization Report
    out << "  \"syntaxCategorization\": {\n";
    out << "    \"tokenTypes\": [\n";
    size_t ttIdx = 0;
    for (const auto& [type, stat] : categorizationReport.tokenTypeStats) {
        if (stat.totalCount == 0) continue;
        if (ttIdx++ > 0) out << ",\n";
        out << "      {\n";
        out << "        \"category\": \"" << escapeJsonString(stat.categoryName) << "\",\n";
        out << "        \"totalCount\": " << stat.totalCount << ",\n";
        out << "        \"uniqueCount\": " << stat.uniqueCount << ",\n";
        out << "        \"streamPercentage\": " << std::fixed << std::setprecision(2) << stat.percentageOfTotal << "\n";
        out << "      }";
    }
    out << "\n    ],\n";

    out << "    \"keywordSubgroups\": [\n";
    size_t sgIdx = 0;
    for (const auto& [kwCat, stat] : categorizationReport.keywordCategoryStats) {
        if (stat.totalCount == 0) continue;
        if (sgIdx++ > 0) out << ",\n";
        out << "      {\n";
        out << "        \"subgroup\": \"" << escapeJsonString(stat.categoryName) << "\",\n";
        out << "        \"totalOccurrences\": " << stat.totalCount << ",\n";
        out << "        \"uniqueCount\": " << stat.uniqueCount << ",\n";
        out << "        \"keywordSharePercentage\": " << std::fixed << std::setprecision(2) << stat.percentageOfTotal << "\n";
        out << "      }";
    }
    out << "\n    ]\n";
    out << "  },\n";

    // 5. Ingested Raw Tokens Sequence (std::vector<std::string>)
    out << "  \"rawTokens\": [\n";
    for (size_t i = 0; i < rawTokens.size(); ++i) {
        out << "    \"" << escapeJsonString(rawTokens[i]) << "\"" << (i + 1 < rawTokens.size() ? "," : "") << "\n";
    }
    out << "  ],\n";

    // 6. Empirical Complexity Benchmarks
    out << "  \"complexityBenchmarks\": [\n";
    for (size_t i = 0; i < benchmarkRows.size(); ++i) {
        const auto& b = benchmarkRows[i];
        double speedup = (b.setTimeMs > 0.0001) ? (b.vectorLinearTimeMs / b.setTimeMs) : 1.0;
        out << "    {\n";
        out << "      \"tokensN\": " << b.tokenCount << ",\n";
        out << "      \"uniqueU\": " << b.uniqueCount << ",\n";
        out << "      \"estimatedTreeHeight\": " << b.estimatedTreeHeight << ",\n";
        out << "      \"vectorLinearTimeMs\": " << std::fixed << std::setprecision(4) << b.vectorLinearTimeMs << ",\n";
        out << "      \"setTimeMs\": " << b.setTimeMs << ",\n";
        out << "      \"unorderedSetTimeMs\": " << b.unorderedSetTimeMs << ",\n";
        out << "      \"speedupVsVector\": " << std::fixed << std::setprecision(2) << speedup << "\n";
        out << "    }" << (i + 1 < benchmarkRows.size() ? "," : "") << "\n";
    }
    out << "  ]\n";

    out << "}\n";
    out.close();
    return true;
}

bool JsonExporter::exportHtmlDashboard(const std::string& htmlOutputPath,
                                       const std::string& jsonFileName) {
    std::string embeddedJson = "{}";
    std::ifstream jsonIn(jsonFileName);
    if (!jsonIn.is_open()) {
        // try prefixing directory if not found
        std::ifstream jsonInAlt("export/" + jsonFileName);
        if (jsonInAlt.is_open()) {
            std::stringstream ss;
            ss << jsonInAlt.rdbuf();
            embeddedJson = ss.str();
        }
    } else {
        std::stringstream ss;
        ss << jsonIn.rdbuf();
        embeddedJson = ss.str();
    }

    std::ofstream out(htmlOutputPath);
    if (!out.is_open()) return false;

    out << R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Compiler Syntax Token Unique Extractor & Categorizer - Dashboard</title>
    <style>
        :root {
            --bg-primary: #0f172a;
            --bg-secondary: #1e293b;
            --bg-card: #1e293b;
            --accent: #38bdf8;
            --accent-glow: rgba(56, 189, 248, 0.2);
            --text-main: #f8fafc;
            --text-muted: #94a3b8;
            --border-color: #334155;
            --success: #10b981;
            --warning: #f59e0b;
            --purple: #a855f7;
        }
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; }
        body { background: var(--bg-primary); color: var(--text-main); min-height: 100vh; padding: 2rem; }
        .header { display: flex; justify-content: space-between; align-items: center; border-bottom: 2px solid var(--border-color); padding-bottom: 1.5rem; margin-bottom: 2rem; }
        .header h1 { font-size: 1.8rem; color: var(--accent); }
        .header .badge { background: #0369a1; padding: 0.35rem 0.8rem; border-radius: 9999px; font-size: 0.85rem; font-weight: 600; }
        .metrics-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 1.2rem; margin-bottom: 2rem; }
        .card { background: var(--bg-card); border: 1px solid var(--border-color); border-radius: 12px; padding: 1.2rem; box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.3); }
        .card .title { font-size: 0.85rem; color: var(--text-muted); text-transform: uppercase; letter-spacing: 0.05em; margin-bottom: 0.4rem; }
        .card .val { font-size: 1.9rem; font-weight: bold; color: var(--accent); }
        .tabs { display: flex; gap: 0.5rem; border-bottom: 1px solid var(--border-color); margin-bottom: 1.5rem; }
        .tab-btn { background: none; border: none; color: var(--text-muted); padding: 0.75rem 1.25rem; font-size: 1rem; cursor: pointer; border-bottom: 3px solid transparent; transition: all 0.2s; }
        .tab-btn.active { color: var(--accent); border-bottom-color: var(--accent); font-weight: 600; }
        .tab-content { display: none; }
        .tab-content.active { display: block; }
        table { width: 100%; border-collapse: collapse; margin-top: 1rem; }
        th, td { padding: 0.8rem 1rem; text-align: left; border-bottom: 1px solid var(--border-color); }
        th { background: #1e293b; color: var(--accent); font-size: 0.85rem; text-transform: uppercase; }
        tr:hover { background: rgba(56, 189, 248, 0.05); }
        .pill { display: inline-block; padding: 0.2rem 0.6rem; border-radius: 6px; font-size: 0.8rem; font-weight: 500; }
        .pill-flow { background: #7c2d12; color: #fdba74; }
        .pill-type { background: #1e3a8a; color: #93c5fd; }
        .pill-oop  { background: #581c87; color: #d8b4fe; }
        .pill-mem  { background: #14532d; color: #86efac; }
        .token-cloud { display: flex; flex-wrap: wrap; gap: 0.5rem; max-height: 400px; overflow-y: auto; padding: 1rem; background: #0b1120; border-radius: 8px; border: 1px solid var(--border-color); }
        .token-item { padding: 0.3rem 0.6rem; background: #1e293b; border-radius: 4px; font-family: monospace; font-size: 0.85rem; color: #e2e8f0; }
        .token-item.is-kw { background: #0284c7; color: #fff; font-weight: bold; }
        .search-box { width: 100%; max-width: 350px; padding: 0.6rem 1rem; background: #0b1120; border: 1px solid var(--border-color); border-radius: 6px; color: #fff; margin-bottom: 1rem; }
    </style>
</head>
<body>
    <div class="header">
        <div>
            <h1>Compiler Syntax Token Unique Extractor & Categorizer</h1>
            <p style="color: var(--text-muted); margin-top: 0.25rem;">Group 5 &bull; Core STL: std::set, std::vector, std::string</p>
        </div>
        <div><span class="badge">C++ Term 1 Project</span></div>
    </div>

    <div class="metrics-grid" id="metricsGrid">
        <div class="card"><div class="title">Total Ingested Tokens</div><div class="val" id="metricTotalTokens">--</div></div>
        <div class="card"><div class="title">Total Keywords</div><div class="val" id="metricTotalKw">--</div></div>
        <div class="card"><div class="title">Unique Keywords (std::set)</div><div class="val" id="metricUniqueKw">--</div></div>
        <div class="card"><div class="title">Duplicate Rejections</div><div class="val" id="metricDuplicates">--</div></div>
        <div class="card"><div class="title">Keyword Density</div><div class="val" id="metricDensity">--%</div></div>
        <div class="card"><div class="title">Duplicate Suppression</div><div class="val" id="metricSuppression">--%</div></div>
    </div>

    <div class="tabs">
        <button class="tab-btn active" onclick="switchTab('keywords')">Unique Keywords & Frequencies</button>
        <button class="tab-btn" onclick="switchTab('categorization')">Syntax Categorization</button>
        <button class="tab-btn" onclick="switchTab('complexity')">Logarithmic Bounds & Benchmarks</button>
        <button class="tab-btn" onclick="switchTab('tokens')">Raw Ingested Tokens Stream</button>
    </div>

    <!-- TAB 1: UNIQUE KEYWORDS -->
    <div id="tab-keywords" class="tab-content active">
        <div class="card">
            <div style="display: flex; justify-content: space-between; align-items: center;">
                <h3>Lexicographically Sorted Keywords Isolated by std::set&lt;std::string&gt;</h3>
                <input type="text" id="kwSearch" class="search-box" placeholder="Filter keywords..." oninput="filterKeywords()">
            </div>
            <table>
                <thead>
                    <tr>
                        <th>#</th>
                        <th>Keyword</th>
                        <th>Category</th>
                        <th>Frequency</th>
                        <th>Keyword Share</th>
                        <th>Stream Share</th>
                    </tr>
                </thead>
                <tbody id="keywordTableBody">
                    <tr><td colspan="6" style="text-align:center;">Loading analysis data...</td></tr>
                </tbody>
            </table>
        </div>
    </div>

    <!-- TAB 2: SYNTAX CATEGORIZATION -->
    <div id="tab-categorization" class="tab-content">
        <div class="card" style="margin-bottom: 1.5rem;">
            <h3>Overall Syntax Categorization Breakdown</h3>
            <table>
                <thead>
                    <tr>
                        <th>Category</th>
                        <th>Total Tokens</th>
                        <th>Unique Items</th>
                        <th>Stream Share</th>
                    </tr>
                </thead>
                <tbody id="catTypeTableBody"></tbody>
            </table>
        </div>
        <div class="card">
            <h3>Keyword Sub-Taxonomy (std::set segregation)</h3>
            <table>
                <thead>
                    <tr>
                        <th>Subgroup</th>
                        <th>Occurrences</th>
                        <th>Unique Count</th>
                        <th>Keyword Share</th>
                    </tr>
                </thead>
                <tbody id="kwSubgroupTableBody"></tbody>
            </table>
        </div>
    </div>

    <!-- TAB 3: LOGARITHMIC BOUNDS & BENCHMARKS -->
    <div id="tab-complexity" class="tab-content">
        <div class="card" style="margin-bottom: 1.5rem;">
            <h3>Theoretical & Empirical Validation of std::set Logarithmic Bounds</h3>
            <p style="color: var(--text-muted); margin: 0.5rem 0 1rem 0;">
                std::set uses a Self-Balancing Red-Black Binary Search Tree. Every insertion verifies uniqueness and rebalances in O(log U) time.
                In contrast, naive std::vector with linear search exhibits O(N &times; U) quadratic decay.
            </p>
            <table>
                <thead>
                    <tr>
                        <th>Tokens (N)</th>
                        <th>Unique (U)</th>
                        <th>RB Max Height &le;</th>
                        <th>std::vector O(N&sup2;) Time</th>
                        <th>std::set O(N log U) Time</th>
                        <th>std::unordered_set O(N) Time</th>
                        <th>std::set vs Vector Speedup</th>
                    </tr>
                </thead>
                <tbody id="benchmarkTableBody"></tbody>
            </table>
        </div>
    </div>

    <!-- TAB 4: RAW TOKENS STREAM -->
    <div id="tab-tokens" class="tab-content">
        <div class="card">
            <h3>Ingested Token Stream (std::vector&lt;std::string&gt;)</h3>
            <p style="color: var(--text-muted); margin-bottom: 1rem;">Tokens are highlighted. Blue badges indicate recognized syntax keywords.</p>
            <div class="token-cloud" id="tokenCloud"></div>
        </div>
    </div>

    <script id="embeddedData" type="application/json">)HTML"
        << embeddedJson << R"HTML(</script>
    <script>
        function switchTab(name) {
            document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
            document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));
            event.target.classList.add('active');
            document.getElementById('tab-' + name).classList.add('active');
        }

        let appData = null;

        function loadData() {
            const embedded = document.getElementById('embeddedData');
            if (embedded && embedded.textContent.trim().length > 2) {
                try {
                    appData = JSON.parse(embedded.textContent);
                    renderData(appData);
                    return;
                } catch (e) {
                    console.log('Parsing embedded JSON failed, falling back to fetch.');
                }
            }
            fetch(')HTML" << jsonFileName << R"HTML(')
                .then(r => r.json())
                .then(data => {
                    appData = data;
                    renderData(data);
                })
                .catch(err => {
                    console.log('Fetching JSON failed, waiting for user input.');
                });
        }

        function renderData(d) {
            document.getElementById('metricTotalTokens').innerText = d.metrics.totalTokensIngested;
            document.getElementById('metricTotalKw').innerText = d.metrics.totalKeywordOccurrences;
            document.getElementById('metricUniqueKw').innerText = d.metrics.uniqueKeywordsCount;
            document.getElementById('metricDuplicates').innerText = d.metrics.duplicateRejectionsCount;
            document.getElementById('metricDensity').innerText = d.metrics.keywordDensityPercentage + '%';
            document.getElementById('metricSuppression').innerText = d.metrics.duplicateSuppressionRatio + '%';

            // Render Keywords
            renderKeywords(d.keywordFrequencies);

            // Render Categories
            const catBody = document.getElementById('catTypeTableBody');
            catBody.innerHTML = '';
            d.syntaxCategorization.tokenTypes.forEach(t => {
                catBody.innerHTML += `<tr>
                    <td><strong>${t.category}</strong></td>
                    <td>${t.totalCount}</td>
                    <td>${t.uniqueCount}</td>
                    <td>${t.streamPercentage}%</td>
                </tr>`;
            });

            const subBody = document.getElementById('kwSubgroupTableBody');
            subBody.innerHTML = '';
            d.syntaxCategorization.keywordSubgroups.forEach(s => {
                subBody.innerHTML += `<tr>
                    <td><strong>${s.subgroup}</strong></td>
                    <td>${s.totalOccurrences}</td>
                    <td>${s.uniqueCount}</td>
                    <td>${s.keywordSharePercentage}%</td>
                </tr>`;
            });

            // Render Benchmarks
            const bBody = document.getElementById('benchmarkTableBody');
            bBody.innerHTML = '';
            d.complexityBenchmarks.forEach(b => {
                bBody.innerHTML += `<tr>
                    <td><strong>${b.tokensN.toLocaleString()}</strong></td>
                    <td>${b.uniqueU.toLocaleString()}</td>
                    <td>${b.estimatedTreeHeight}</td>
                    <td>${b.vectorLinearTimeMs.toFixed(3)} ms</td>
                    <td style="color:var(--accent); font-weight:bold;">${b.setTimeMs.toFixed(3)} ms</td>
                    <td>${b.unorderedSetTimeMs.toFixed(3)} ms</td>
                    <td style="color:var(--success); font-weight:bold;">${b.speedupVsVector.toFixed(1)}x faster</td>
                </tr>`;
            });

            // Render Tokens
            const cloud = document.getElementById('tokenCloud');
            cloud.innerHTML = '';
            const kwSet = new Set(d.uniqueKeywordsLexicographical);
            d.rawTokens.forEach(tok => {
                const isKw = kwSet.has(tok);
                const span = document.createElement('span');
                span.className = 'token-item' + (isKw ? ' is-kw' : '');
                span.innerText = tok;
                cloud.appendChild(span);
            });
        }

        function renderKeywords(metrics) {
            const tbody = document.getElementById('keywordTableBody');
            tbody.innerHTML = '';
            metrics.forEach((m, idx) => {
                tbody.innerHTML += `<tr>
                    <td>${idx + 1}</td>
                    <td><strong style="color:var(--accent);">${m.keyword}</strong></td>
                    <td><span class="pill pill-type">${m.category}</span></td>
                    <td><strong>${m.frequency}</strong></td>
                    <td>${m.relativeFrequencyPercentage.toFixed(2)}%</td>
                    <td>${m.streamPercentage.toFixed(2)}%</td>
                </tr>`;
            });
        }

        function filterKeywords() {
            if (!appData) return;
            const query = document.getElementById('kwSearch').value.toLowerCase();
            const filtered = appData.keywordFrequencies.filter(m => 
                m.keyword.toLowerCase().includes(query) || m.category.toLowerCase().includes(query)
            );
            renderKeywords(filtered);
        }

        window.onload = loadData;
    </script>
</body>
</html>
)HTML";

    out.close();
    return true;
}
