// Minimalist Niche Monochromatic 3D Visualizer & Application Engine (Group 5)
// Core STL: std::set<std::string>, std::vector<std::string>, Red-Black Tree Bounds
// Strict Palette: Pure Black, Shades of Gray, Pure White

const DEFAULT_DEMO = `// Default Demonstration C++ Program for Token Extraction
#include <iostream>
#include <vector>
#include <string>

namespace ParserDemo {
    template <typename T>
    class DynamicContainer {
    private:
        std::vector<T> elements;
        const size_t capacityLimit = 1000;
        static int totalContainers;

    public:
        DynamicContainer() {
            totalContainers++;
        }

        virtual ~DynamicContainer() noexcept {
            totalContainers--;
        }

        void addElement(const T& item) {
            if (elements.size() >= capacityLimit) {
                throw std::runtime_error("Capacity limit exceeded");
            }
            elements.push_back(item);
        }

        auto getSize() const noexcept -> size_t {
            return elements.size();
        }
    };

    template <typename T>
    int DynamicContainer<T>::totalContainers = 0;
}

int main(int argc, char* argv[]) {
    using namespace ParserDemo;
    DynamicContainer<int>* containerPtr = new DynamicContainer<int>();
    
    for (int i = 0; i < 50; ++i) {
        if (i % 2 == 0) {
            containerPtr->addElement(i * 10);
        } else {
            continue;
        }
    }

    std::cout << "Container size: " << containerPtr->getSize() << std::endl;
    delete containerPtr;
    containerPtr = nullptr;
    return 0;
}
`;

let currentAnalysis = null;
let visualizerMode = 'tree';

// =============================================================================
// THREE.JS 3D MONOCHROMATIC ENGINE
// =============================================================================
let scene, camera, renderer;
let treeGroup, vortexGroup;
let isDragging = false;
let previousMousePosition = { x: 0, y: 0 };
let targetRotation = { x: 0.15, y: -0.2 };

function init3DVisualizer() {
    const canvas = document.getElementById('treeCanvas');
    const container = document.getElementById('canvasContainer');
    if (!canvas || !container || typeof THREE === 'undefined') return;

    const width = container.clientWidth;
    const height = container.clientHeight || 400;

    scene = new THREE.Scene();
    scene.fog = new THREE.FogExp2(0x000000, 0.012);

    camera = new THREE.PerspectiveCamera(45, width / height, 0.1, 1000);
    camera.position.set(0, 5, 55);

    renderer = new THREE.WebGLRenderer({ canvas: canvas, antialias: true, alpha: true });
    renderer.setSize(width, height);
    renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));

    // Pure White Lighting & Subtle Ambient Shadow
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.55);
    scene.add(ambientLight);

    const keyLight = new THREE.DirectionalLight(0xffffff, 1.4);
    keyLight.position.set(25, 45, 35);
    scene.add(keyLight);

    const fillLight = new THREE.DirectionalLight(0xaaaaaa, 0.7);
    fillLight.position.set(-30, -20, -25);
    scene.add(fillLight);

    treeGroup = new THREE.Group();
    vortexGroup = new THREE.Group();
    scene.add(treeGroup);
    scene.add(vortexGroup);

    // Mouse Controls (3D Orbit & Pan)
    canvas.addEventListener('mousedown', (e) => {
        isDragging = true;
        previousMousePosition = { x: e.clientX, y: e.clientY };
    });

    window.addEventListener('mousemove', (e) => {
        if (!isDragging) return;
        const deltaX = e.clientX - previousMousePosition.x;
        const deltaY = e.clientY - previousMousePosition.y;

        targetRotation.y += deltaX * 0.006;
        targetRotation.x += deltaY * 0.006;
        targetRotation.x = Math.max(-Math.PI / 3, Math.min(Math.PI / 3, targetRotation.x));

        previousMousePosition = { x: e.clientX, y: e.clientY };
    });

    window.addEventListener('mouseup', () => {
        isDragging = false;
    });

    canvas.addEventListener('wheel', (e) => {
        e.preventDefault();
        camera.position.z = Math.max(15, Math.min(130, camera.position.z + e.deltaY * 0.06));
    }, { passive: false });

    window.addEventListener('resize', () => {
        if (!container || !renderer || !camera) return;
        const w = container.clientWidth;
        const h = container.clientHeight || 400;
        camera.aspect = w / h;
        camera.updateProjectionMatrix();
        renderer.setSize(w, h);
    });

    animate3D();
}

function animate3D() {
    requestAnimationFrame(animate3D);

    if (treeGroup) {
        treeGroup.rotation.y += (targetRotation.y - treeGroup.rotation.y) * 0.08;
        treeGroup.rotation.x += (targetRotation.x - treeGroup.rotation.x) * 0.08;
    }

    if (vortexGroup && visualizerMode === 'vortex') {
        vortexGroup.rotation.y += 0.012;
        vortexGroup.rotation.x += 0.004;
    }

    if (renderer && scene && camera) {
        renderer.render(scene, camera);
    }
}

function reset3DCamera() {
    targetRotation = { x: 0.15, y: -0.2 };
    if (camera) camera.position.set(0, 5, 55);
}

function setVisualizerMode(mode) {
    visualizerMode = mode;
    document.getElementById('btnModeTree').classList.toggle('active', mode === 'tree');
    document.getElementById('btnModeVortex').classList.toggle('active', mode === 'vortex');

    if (treeGroup) treeGroup.visible = (mode === 'tree');
    if (vortexGroup) vortexGroup.visible = (mode === 'vortex');
}

// Minimalist Monochrome Canvas Text Sprite
function makeTextSprite(message, isPrimary = true) {
    const fontface = 'Fira Code, monospace';
    const fontsize = 26;
    const canvas = document.createElement('canvas');
    canvas.width = 256;
    canvas.height = 70;
    const ctx = canvas.getContext('2d');

    ctx.font = `600 ${fontsize}px ${fontface}`;
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';

    if (isPrimary) {
        // Crisp White Pill with Black Text
        ctx.fillStyle = '#ffffff';
        ctx.strokeStyle = '#ffffff';
        ctx.lineWidth = 1;
        ctx.beginPath();
        ctx.roundRect(10, 8, 236, 54, 4);
        ctx.fill();
        ctx.stroke();

        ctx.fillStyle = '#000000';
        ctx.fillText(message, 128, 35);
    } else {
        // Pure Black Pill with White Text and Silver Border
        ctx.fillStyle = '#0a0a0a';
        ctx.strokeStyle = '#444444';
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.roundRect(10, 8, 236, 54, 4);
        ctx.fill();
        ctx.stroke();

        ctx.fillStyle = '#f0f0f0';
        ctx.fillText(message, 128, 35);
    }

    const texture = new THREE.CanvasTexture(canvas);
    texture.minFilter = THREE.LinearFilter;
    const spriteMaterial = new THREE.SpriteMaterial({ map: texture, transparent: true });
    const sprite = new THREE.Sprite(spriteMaterial);
    sprite.scale.set(6.5, 1.8, 1);
    return sprite;
}

// Build 3D Monochromatic Balanced Binary Tree (std::set)
function build3DTree(uniqueKeywords) {
    if (!treeGroup || typeof THREE === 'undefined') return;

    while (treeGroup.children.length > 0) {
        treeGroup.remove(treeGroup.children[0]);
    }

    if (!uniqueKeywords || uniqueKeywords.length === 0) {
        document.getElementById('hudNodeCount').innerText = '0';
        document.getElementById('hudTreeDepth').innerText = '0';
        return;
    }

    const n = uniqueKeywords.length;
    document.getElementById('hudNodeCount').innerText = n.toString();

    const maxRbDepth = Math.ceil(2 * Math.log2(n + 1));
    document.getElementById('hudTreeDepth').innerText = `\u2264 ${maxRbDepth}`;

    function buildBst(start, end, depth, x, y, z, spreadX) {
        if (start > end) return null;

        const mid = Math.floor((start + end) / 2);
        const keyword = uniqueKeywords[mid];
        const isPrimary = (depth > 0 && depth % 2 === 1);

        const nodeGroup = new THREE.Group();
        nodeGroup.position.set(x, y, z);

        // Spheres in pure monochrome
        const sphereGeo = new THREE.SphereGeometry(1.3, 28, 28);
        const sphereMat = new THREE.MeshStandardMaterial({
            color: isPrimary ? 0xffffff : 0x1a1a1a,
            metalness: isPrimary ? 0.2 : 0.8,
            roughness: isPrimary ? 0.15 : 0.35,
            emissive: isPrimary ? 0x222222 : 0x050505,
            emissiveIntensity: 0.3
        });
        const sphere = new THREE.Mesh(sphereGeo, sphereMat);
        nodeGroup.add(sphere);

        // Minimalist Wireframe Halo Ring
        const ringGeo = new THREE.RingGeometry(1.6, 1.8, 28);
        const ringMat = new THREE.MeshBasicMaterial({
            color: isPrimary ? 0xffffff : 0x555555,
            side: THREE.DoubleSide,
            transparent: true,
            opacity: 0.7
        });
        const ring = new THREE.Mesh(ringGeo, ringMat);
        ring.rotation.x = Math.PI / 2;
        nodeGroup.add(ring);

        // Text Sprite
        const textSprite = makeTextSprite(keyword, isPrimary);
        textSprite.position.set(0, 2.6, 0);
        nodeGroup.add(textSprite);

        treeGroup.add(nodeGroup);

        const childSpread = spreadX * 0.55;
        const deltaY = 6.2;

        if (start <= mid - 1) {
            buildBst(start, mid - 1, depth + 1, x - spreadX, y - deltaY, z, childSpread);
            connectNodes(nodeGroup.position, new THREE.Vector3(x - spreadX, y - deltaY, z));
        }

        if (mid + 1 <= end) {
            buildBst(mid + 1, end, depth + 1, x + spreadX, y - deltaY, z, childSpread);
            connectNodes(nodeGroup.position, new THREE.Vector3(x + spreadX, y - deltaY, z));
        }

        return nodeGroup;
    }

    function connectNodes(posA, posB) {
        const points = [posA, posB];
        const lineGeo = new THREE.BufferGeometry().setFromPoints(points);
        const lineMat = new THREE.LineBasicMaterial({
            color: 0x666666,
            transparent: true,
            opacity: 0.65,
            linewidth: 1
        });
        const line = new THREE.Line(lineGeo, lineMat);
        treeGroup.add(line);
    }

    const rootY = 12.0;
    const initialSpread = Math.min(22.0, Math.max(12.0, n * 0.75));
    buildBst(0, n - 1, 0, 0, rootY, 0, initialSpread);

    camera.position.set(0, 2, Math.max(45, n * 1.5));
}

// 3D Monochromatic Token Vortex
function build3DTokenVortex(rawTokens) {
    if (!vortexGroup || typeof THREE === 'undefined') return;

    while (vortexGroup.children.length > 0) {
        vortexGroup.remove(vortexGroup.children[0]);
    }

    if (!rawTokens || rawTokens.length === 0) return;

    const count = Math.min(180, rawTokens.length);
    const boxGeo = new THREE.BoxGeometry(0.85, 0.85, 0.85);

    for (let i = 0; i < count; ++i) {
        const u = i / count;
        const radius = 6 + u * 20;
        const angle = u * Math.PI * 10;
        const x = Math.cos(angle) * radius;
        const z = Math.sin(angle) * radius;
        const y = (u - 0.5) * 35;

        const isWhite = (i % 3 === 0);
        const isSilver = (i % 3 === 1);
        const mat = new THREE.MeshStandardMaterial({
            color: isWhite ? 0xffffff : (isSilver ? 0x999999 : 0x1f1f1f),
            metalness: 0.6,
            roughness: 0.2
        });

        const mesh = new THREE.Mesh(boxGeo, mat);
        mesh.position.set(x, y, z);
        mesh.rotation.set(Math.random() * Math.PI, Math.random() * Math.PI, 0);
        vortexGroup.add(mesh);
    }

    vortexGroup.visible = (visualizerMode === 'vortex');
}

// =============================================================================
// CLIENT APPLICATION & API BRIDGE
// =============================================================================
function switchTab(tabId) {
    document.querySelectorAll('.mono-tab-item').forEach(btn => btn.classList.remove('active'));
    document.querySelectorAll('.mono-tab-pane').forEach(pane => pane.classList.remove('active'));

    event.currentTarget.classList.add('active');
    const targetPane = document.getElementById('tab-' + tabId);
    if (targetPane) targetPane.classList.add('active');
}

function loadPresetSample() {
    const val = document.getElementById('sampleSelect').value;
    if (val === 'demo') {
        document.getElementById('codeEditor').value = DEFAULT_DEMO;
        analyzeCode();
        return;
    }

    fetch(`/api/sample?id=${val}`)
        .then(r => r.json())
        .then(data => {
            if (data.code) {
                document.getElementById('codeEditor').value = data.code;
                analyzeCode();
            }
        })
        .catch(err => {
            console.error('Failed to load preset sample:', err);
        });
}

function clearCode() {
    document.getElementById('codeEditor').value = '';
}

function analyzeCode() {
    const code = document.getElementById('codeEditor').value;
    if (!code.trim()) {
        alert('Please enter or select some C++ source code to analyze.');
        return;
    }

    const btn = document.getElementById('btnAnalyze');
    const origHtml = btn.innerHTML;
    btn.innerHTML = 'PROCESSING...';
    btn.disabled = true;

    fetch('/api/analyze', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ code: code })
    })
    .then(r => r.json())
    .then(data => {
        btn.innerHTML = origHtml;
        btn.disabled = false;

        if (data.error) {
            alert('Analysis Error: ' + data.error);
            return;
        }

        currentAnalysis = data;
        renderAnalysis(data);
    })
    .catch(err => {
        btn.innerHTML = origHtml;
        btn.disabled = false;
        alert('Failed to connect to local server: ' + err.message);
    });
}

function renderAnalysis(data) {
    const m = data.metrics;
    document.getElementById('cardTokens').innerText = m.totalTokensIngested.toLocaleString();
    document.getElementById('cardUniqueKw').innerText = m.uniqueKeywordsCount.toLocaleString();
    document.getElementById('cardDuplicates').innerText = m.duplicateRejectionsCount.toLocaleString();
    document.getElementById('cardDensity').innerText = m.keywordDensityPercentage.toFixed(1) + '%';
    document.getElementById('cardSuppression').innerText = m.duplicateSuppressionRatio.toFixed(1) + '%';

    build3DTree(data.uniqueKeywordsLexicographical);
    build3DTokenVortex(data.rawTokens);

    renderKeywords(data.keywordFrequencies);
    renderCategorization(data.syntaxCategorization);

    if (data.complexityBenchmarks && data.complexityBenchmarks.length > 0) {
        renderBenchmarks(data.complexityBenchmarks);
    }

    renderTokenStream(data.rawTokens, data.uniqueKeywordsLexicographical);

    if (data.cliOutput) {
        document.getElementById('terminalOutput').innerText = data.cliOutput;
    }
}

// Strict Monochromatic Badges
function getCategoryBadge(category) {
    const cat = category.toLowerCase();
    if (cat.includes('control')) return '<span class="mono-badge badge-solid-white">Control Flow</span>';
    if (cat.includes('type') || cat.includes('primitive')) return '<span class="mono-badge badge-outline">Data Type</span>';
    if (cat.includes('modifier') || cat.includes('storage')) return '<span class="mono-badge badge-solid-dark">Storage / Mod</span>';
    if (cat.includes('oop') || cat.includes('access') || cat.includes('struct')) return '<span class="mono-badge badge-outline">OOP / Struct</span>';
    if (cat.includes('memory') || cat.includes('exception')) return '<span class="mono-badge badge-solid-white">Memory / Exc</span>';
    if (cat.includes('template') || cat.includes('cast')) return '<span class="mono-badge badge-solid-dark">Template / Cast</span>';
    return `<span class="mono-badge badge-outline">${category}</span>`;
}

function renderKeywords(metrics) {
    const tbody = document.getElementById('keywordTableBody');
    if (!metrics || metrics.length === 0) {
        tbody.innerHTML = '<tr><td colspan="6" class="mono-empty-state">No syntax keywords found in source.</td></tr>';
        return;
    }

    let html = '';
    metrics.forEach((m, idx) => {
        html += `<tr>
            <td style="color:var(--text-dim); font-family:var(--font-mono);">${idx + 1}</td>
            <td><strong style="color:var(--text-white); font-family:var(--font-mono); font-size:0.95rem;">${m.keyword}</strong></td>
            <td>${getCategoryBadge(m.category)}</td>
            <td><strong style="color:var(--text-white); font-size:1rem; font-family:var(--font-mono);">${m.frequency}</strong></td>
            <td style="font-family:var(--font-mono);">${m.relativeFrequencyPercentage.toFixed(2)}%</td>
            <td style="color:var(--text-silver); font-family:var(--font-mono);">${m.streamPercentage.toFixed(2)}%</td>
        </tr>`;
    });
    tbody.innerHTML = html;
}

function filterKeywordTable() {
    if (!currentAnalysis || !currentAnalysis.keywordFrequencies) return;
    const query = document.getElementById('kwSearchInput').value.toLowerCase();
    const filtered = currentAnalysis.keywordFrequencies.filter(m => 
        m.keyword.toLowerCase().includes(query) || m.category.toLowerCase().includes(query)
    );
    renderKeywords(filtered);
}

function renderCategorization(catData) {
    const tBody = document.getElementById('categoryTableBody');
    if (catData && catData.tokenTypes) {
        let html = '';
        catData.tokenTypes.forEach(t => {
            html += `<tr>
                <td><strong>${t.category}</strong></td>
                <td style="font-family:var(--font-mono);">${t.totalCount}</td>
                <td><span class="mono-badge badge-outline">${t.uniqueCount} distinct</span></td>
                <td style="font-family:var(--font-mono);">${t.streamPercentage.toFixed(2)}%</td>
            </tr>`;
        });
        tBody.innerHTML = html;
    }

    const sBody = document.getElementById('kwSubgroupTableBody');
    if (catData && catData.keywordSubgroups) {
        let html = '';
        catData.keywordSubgroups.forEach(s => {
            html += `<tr>
                <td>${getCategoryBadge(s.subgroup)}</td>
                <td style="font-family:var(--font-mono);"><strong>${s.totalOccurrences}</strong></td>
                <td><span class="mono-badge badge-outline">${s.uniqueCount} distinct</span></td>
                <td style="font-family:var(--font-mono);">${s.keywordSharePercentage.toFixed(2)}%</td>
            </tr>`;
        });
        sBody.innerHTML = html;
    }
}

function renderBenchmarks(rows) {
    const tbody = document.getElementById('benchmarkTableBody');
    if (!rows || rows.length === 0) return;

    let html = '';
    rows.forEach(b => {
        html += `<tr>
            <td style="font-family:var(--font-mono);"><strong>${b.tokensN.toLocaleString()}</strong></td>
            <td style="font-family:var(--font-mono);">${b.uniqueU.toLocaleString()}</td>
            <td><span class="mono-badge badge-outline">&le; ${b.estimatedTreeHeight}</span></td>
            <td style="color:var(--text-muted); font-family:var(--font-mono);">${b.vectorLinearTimeMs.toFixed(3)} ms</td>
            <td style="color:var(--text-white); font-weight:700; font-family:var(--font-mono);">${b.setTimeMs.toFixed(3)} ms</td>
            <td style="color:var(--text-silver); font-family:var(--font-mono);">${b.unorderedSetTimeMs.toFixed(3)} ms</td>
            <td><strong style="color:var(--text-white); font-size:0.95rem; font-family:var(--font-mono);">${b.speedupVsVector.toFixed(1)}&times; FASTER</strong></td>
        </tr>`;
    });
    tbody.innerHTML = html;
}

function renderTokenStream(rawTokens, uniqueKws) {
    const cloud = document.getElementById('tokenCloud');
    document.getElementById('tokenStreamCount').innerText = `${rawTokens.length} TOKENS`;
    cloud.innerHTML = '';

    const kwSet = new Set(uniqueKws);
    rawTokens.forEach((tok, idx) => {
        const isKw = kwSet.has(tok);
        const chip = document.createElement('span');
        chip.className = 'mono-token-chip' + (isKw ? ' is-kw' : '');
        chip.title = `Token #${idx}: ${tok}` + (isKw ? ' [KEYWORD]' : '');
        chip.innerHTML = `<span class="chip-idx">${idx}</span>${escapeHtml(tok)}`;
        cloud.appendChild(chip);
    });
}

function runBenchmarkSuite() {
    const btn = document.getElementById('btnBenchmark');
    btn.innerHTML = 'BENCHMARKING...';
    btn.disabled = true;

    fetch('/api/benchmark', { method: 'POST' })
        .then(r => r.json())
        .then(data => {
            btn.innerHTML = 'RUN O(LOG N) BENCHMARK';
            btn.disabled = false;
            if (data.benchmarks) {
                renderBenchmarks(data.benchmarks);
                switchTab('benchmarks');
                document.querySelectorAll('.mono-tab-item').forEach(b => {
                    if (b.innerText.includes('LOGARITHMIC')) b.classList.add('active');
                    else b.classList.remove('active');
                });
            }
        })
        .catch(err => {
            btn.innerHTML = 'RUN O(LOG N) BENCHMARK';
            btn.disabled = false;
            alert('Benchmark failed: ' + err.message);
        });
}

function escapeHtml(str) {
    return str.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
}

window.addEventListener('DOMContentLoaded', () => {
    init3DVisualizer();
    document.getElementById('codeEditor').value = DEFAULT_DEMO;
    analyzeCode();
});
