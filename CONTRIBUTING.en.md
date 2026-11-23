# How to Contribute to Dirac-Solver

> 🌐 **Language / Idioma:** **[Español](CONTRIBUTING.md)** ([`CONTRIBUTING.es.md`](CONTRIBUTING.es.md)) | **English**

Thank you for your interest in contributing to **Dirac-Solver**! Your help is very welcome. To keep the workflow smooth and clear for everyone, please follow these contribution guidelines.

## Types of Contributions

We welcome various types of contributions, including:
* Bug reports.
* Feature requests or performance improvement suggestions.
* Code contributions (bug fixes or new features in C++ or Python).
* Documentation improvements and example notebooks (in both Spanish and English).

## Reporting Bugs and Suggestions

To report a bug or suggest an enhancement, please **use the GitHub "Issues" tab**.

* **Check existing issues:** Before creating a new issue, check whether someone else has already reported the same problem or proposed a similar idea.
* **Be clear and specific:**
    * **For Bugs:** Describe the issue, the steps to reproduce it, your environment (OS, Python version, C++ compiler), and the expected vs. actual behavior.
    * **For Suggestions:** Explain your idea, its physical or computational motivation, and why it would be useful for the project.

## Contributing Code (Pull Requests)

Please follow this workflow when submitting code changes:

### 1. Set Up Your Environment

Make sure your local development environment is ready.

1.  **Fork** the `Dirac-Equation-Solver` repository to your own GitHub account.
2.  **Clone** your fork to your local machine:
    ```bash
    git clone https://github.com/YOUR_USERNAME/Dirac-Equation-Solver.git
    cd Dirac-Solver
    ```
    *(Replace `YOUR_USERNAME` with your GitHub username)*.

3.  **Install the project in editable mode.** This compiles the `_core` C++ extension via `scikit-build-core` and lets you test your changes locally:
    ```bash
    pip install -e .
    ```

### 2. Make Your Changes

1.  **Create a new branch** for your changes. Avoid working directly on `main`:
    ```bash
    git checkout -b feature/my-new-feature
    ```
    *(Use a descriptive branch name such as `fix/matrix-error` or `feature/new-potential`)*.

2.  **Write your code** and run the test suite with `pytest` to make sure no regressions were introduced:
    ```bash
    pytest
    ```

3.  **Update the documentation (if needed).** If your change affects how the code is used or introduces a new feature, please update the bilingual documentation (`docs/en/` and `docs/es/`) as well as Doxygen docstrings. You can build the documentation locally to preview it:
    ```bash
    doxygen Doxyfile.en
    cd ./docs/doxygen_en/latex
    make
    ```
    *(For the Spanish Doxygen documentation, run `doxygen Doxyfile`)*.

### 3. Submit Your Changes

1.  **Commit** your changes with a clear, semantic commit message:
    ```bash
    git add .
    git commit -m "feat: Add method X to compute Y"
    ```
2.  **Push your branch** to your fork on GitHub:
    ```bash
    git push origin feature/my-new-feature
    ```
3.  **Open a Pull Request (PR)** on GitHub from your branch to the `main` branch of the upstream `Dirac-Solver` repository.
4.  **Describe your PR:** Explain what changes you made and why. If your PR resolves an existing Issue, reference it (e.g., `"Closes #12"`).

That's it! We will review your contribution as soon as possible.
