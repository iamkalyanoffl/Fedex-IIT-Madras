import requests
import os
import re

GITHUB_TOKEN = os.getenv('GITHUB_TOKEN')
REPO = os.getenv('GITHUB_REPOSITORY')  
PR_NUMBER = os.getenv('PR_NUMBER')

REPO_OWNER, REPO_NAME = REPO.split('/')

benchmark_results = [
  {'master': '0.002s', 'pr': '0.0018s'},
  {'master': '0.003s', 'pr': '0.0025s'},
  {'master': '0.004s', 'pr': '0.0038s'}
]

def create_markdown_table(results):
    header = "| Master | PR |\n|--------|----|"
    rows = [f"| {result['master']} | {result['pr']} |" for result in results]
    return f"{header}\n" + "\n".join(rows)

def get_pr_comments(repo_owner, repo_name, pr_number):
    url = f"https://api.github.com/repos/{repo_owner}/{repo_name}/issues/{pr_number}/comments"
    headers = {'Authorization': f'token {GITHUB_TOKEN}'}
    response = requests.get(url, headers=headers)
    response.raise_for_status()
    return response.json()

def update_comment(comment_id, repo_owner, repo_name, body):
    url = f"https://api.github.com/repos/{repo_owner}/{repo_name}/issues/comments/{comment_id}"
    headers = {'Authorization': f'token {GITHUB_TOKEN}'}
    data = {'body': body}
    response = requests.patch(url, headers=headers, json=data)
    response.raise_for_status()
    return response.json()

def main():
    comments = get_pr_comments(REPO_OWNER, REPO_NAME, PR_NUMBER)
    if comments and len(comments) > 0:
        first_comment = comments[0]
        markdown_table = create_markdown_table(benchmark_results)
        new_benchmark_section = f"<!-- BENCHMARK_RESULTS_START -->\n## Benchmark Results\n{markdown_table}\n<!-- BENCHMARK_RESULTS_END -->"

        if re.search(r'<!-- BENCHMARK_RESULTS_START -->.*<!-- BENCHMARK_RESULTS_END -->', first_comment['body'], re.DOTALL):
            updated_body = re.sub(
                r'<!-- BENCHMARK_RESULTS_START -->.*<!-- BENCHMARK_RESULTS_END -->',
                new_benchmark_section,
                first_comment['body'],
                flags=re.DOTALL
            )
        else:
            updated_body = f"{first_comment['body']}\n\n{new_benchmark_section}"
        
        update_comment(first_comment['id'], REPO_OWNER, REPO_NAME, updated_body)
        print("PR comment updated successfully.")
    else:
        print("No comments found on this PR.")
        exit(1)

if __name__ == "__main__":
    main()
