#!/usr/bin/env python3
"""
Localhost Web Server for Compiler Syntax Token Unique Extractor & Categorizer
Serves interactive Frontend on http://localhost:8080 and bridges to C++ backend executable.
"""

import http.server
import socketserver
import os
import sys
import json
import subprocess
import urllib.parse

PORT = 8080
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
WEB_DIR = os.path.join(BASE_DIR, "web")
BIN_EXE = os.path.join(BASE_DIR, "bin", "syntax_analyzer.exe")
EXPORT_DIR = os.path.join(BASE_DIR, "export")

os.makedirs(EXPORT_DIR, exist_ok=True)
os.makedirs(WEB_DIR, exist_ok=True)

class ApiHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=WEB_DIR, **kwargs)

    def end_headers(self):
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        super().end_headers()

    def do_OPTIONS(self):
        self.send_response(200)
        self.end_headers()

    def do_GET(self):
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path

        if path == "/api/status":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            resp = {
                "status": "online",
                "binary": os.path.exists(BIN_EXE),
                "port": PORT,
                "project": "Compiler Syntax Token Unique Extractor & Categorizer"
            }
            self.wfile.write(json.dumps(resp).encode("utf-8"))
            return

        elif path == "/api/sample":
            query = urllib.parse.parse_qs(parsed.query)
            sample_id = query.get("id", ["1"])[0]
            sample_file_map = {
                "1": os.path.join(BASE_DIR, "samples", "sample1_simple.cpp"),
                "2": os.path.join(BASE_DIR, "samples", "sample2_complex.cpp"),
                "3": os.path.join(BASE_DIR, "samples", "sample3_keywords_stress.cpp"),
            }
            target_path = sample_file_map.get(sample_id, sample_file_map["1"])
            code = ""
            if os.path.exists(target_path):
                with open(target_path, "r", encoding="utf-8") as f:
                    code = f.read()

            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(json.dumps({"sampleId": sample_id, "code": code}).encode("utf-8"))
            return

        # Default static file serving from web/
        return super().do_GET()

    def do_POST(self):
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path

        content_length = int(self.headers.get("Content-Length", 0))
        post_data = self.rfile.read(content_length)

        if path == "/api/analyze":
            try:
                payload = json.loads(post_data.decode("utf-8"))
                code = payload.get("code", "")

                temp_cpp = os.path.join(EXPORT_DIR, "temp_source.cpp")
                temp_json = os.path.join(EXPORT_DIR, "analysis_result.json")

                with open(temp_cpp, "w", encoding="utf-8") as f:
                    f.write(code)

                # Execute C++ syntax_analyzer.exe
                cmd = [BIN_EXE, "--file", temp_cpp, "--json", temp_json]
                result = subprocess.run(cmd, capture_output=True, text=True, cwd=BASE_DIR)

                if os.path.exists(temp_json):
                    with open(temp_json, "r", encoding="utf-8") as f:
                        data = json.load(f)
                    data["cliOutput"] = result.stdout
                    self.send_response(200)
                    self.send_header("Content-Type", "application/json")
                    self.end_headers()
                    self.wfile.write(json.dumps(data).encode("utf-8"))
                else:
                    self.send_response(500)
                    self.send_header("Content-Type", "application/json")
                    self.end_headers()
                    self.wfile.write(json.dumps({
                        "error": "Backend execution failed to generate JSON",
                        "stderr": result.stderr,
                        "stdout": result.stdout
                    }).encode("utf-8"))

            except Exception as e:
                self.send_response(500)
                self.send_header("Content-Type", "application/json")
                self.end_headers()
                self.wfile.write(json.dumps({"error": str(e)}).encode("utf-8"))
            return

        elif path == "/api/benchmark":
            try:
                # Trigger benchmark in C++ binary
                cmd = [BIN_EXE, "--demo"]
                subprocess.run(cmd, capture_output=True, text=True, cwd=BASE_DIR)

                result_json = os.path.join(EXPORT_DIR, "analysis_result.json")
                if os.path.exists(result_json):
                    with open(result_json, "r", encoding="utf-8") as f:
                        data = json.load(f)
                    self.send_response(200)
                    self.send_header("Content-Type", "application/json")
                    self.end_headers()
                    self.wfile.write(json.dumps({
                        "benchmarks": data.get("complexityBenchmarks", [])
                    }).encode("utf-8"))
                else:
                    self.send_response(500)
                    self.send_header("Content-Type", "application/json")
                    self.end_headers()
                    self.wfile.write(json.dumps({"error": "Failed to read benchmark results"}).encode("utf-8"))
            except Exception as e:
                self.send_response(500)
                self.send_header("Content-Type", "application/json")
                self.end_headers()
                self.wfile.write(json.dumps({"error": str(e)}).encode("utf-8"))
            return

        self.send_response(404)
        self.end_headers()

def run_server():
    socketserver.TCPServer.allow_reuse_address = True
    with socketserver.TCPServer(("0.0.0.0", PORT), ApiHandler) as httpd:
        print(f"=================================================================")
        print(f"  [+] Localhost Web Server started successfully!")
        print(f"  [>] Access UI at: http://localhost:{PORT}")
        print(f"  [*] C++ Binary   : {BIN_EXE}")
        print(f"=================================================================")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n[-] Server shutting down.")
            httpd.shutdown()

if __name__ == "__main__":
    run_server()
