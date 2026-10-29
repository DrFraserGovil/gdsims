\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{cortex}[CorTeX note class]
\newif\ifcortex@svg
\cortex@svgtrue
\def\cortex@width{25cm}
\def\cortex@margin{8pt}
\DeclareKeys[cortex]{
	svg .code = \cortex@svgtrue,
	pdf .code = \cortex@svgfalse,
	width .store = \cortex@width,
	margin .store = \cortex@margin,
	unknown .code = \PassOptionsToClass{\CurrentOption}{standalone}
}
\ProcessKeyOptions[cortex]
\ifcortex@svg
	\def\pgfsysdriver{pgfsys-dvisvgm.def}
\fi
% a margin on every page (the margin setting): without one, glyphs touch the edge of the SVG and the browser clips their tops,
% and anything drawn outside the text width (such as a tcolorbox with oversize, whose frame hangs out by its rule and padding) is cut off
\edef\cortex@border{\noexpand\PassOptionsToClass{border=\cortex@margin}{standalone}}
\cortex@border
\LoadClass{standalone}

% The engine wraps every note body in this, so all notes share one fixed width (and narrower content can be centred within it).
% minipage resets \parindent and \parskip to zero, so the values set by the preambles are captured before it opens, and restored inside.
\newlength{\cortex@parindent}
\newskip\cortex@parskip
\newenvironment{cortexbody}
{%
	\setlength{\cortex@parindent}{\parindent}%
	\setlength{\cortex@parskip}{\parskip}%
	\begin{minipage}{\cortex@width}%
	\setlength{\parindent}{\cortex@parindent}%
	\setlength{\parskip}{\cortex@parskip}%
}
{\end{minipage}}

% With the InsertTitle setting, every note begins \CortexTitle{<title>}; users may \renewcommand it
\newcommand{\CortexTitle}[1]{{\Large\bfseries #1\par}\medskip}

% [[links]] are rewritten by the engine into \CortexLink{<origin>}{<slug>}{<text>}, where <origin> is the key of the note the link was written
% in: a link always resolves from where it was written, even inside a transcluded note.
% SVG output: the text is wrapped in an <a> pointing at the viewer's route for that origin and slug (a raw dvisvgm special: no hyperref, no
% border box, and a link which wraps across lines stays one anchor); the viewer resolves it and styles the link.
% PDF output: there is no viewer, so the engine resolves every link before compiling, and declares each target with
% \CortexLinkTarget{<origin>}{<slug>}{<path to the target PDF, relative to this one, hex-encoded so no filename can upset TeX>}.
% A resolved link becomes a GoToR annotation (a raw xdvipdfmx special: "open that file"), followed by PDF readers such as Okular;
% an unresolved one is just styled. Users may \renewcommand \CortexLinkText / \CortexUnresolvedLink (or \CortexLink itself) in their preamble.
\newcommand{\CortexLinkTarget}[3]{\expandafter\def\csname cortex@target@#1@#2\endcsname{#3}}
\ifcortex@svg
	\newcommand{\CortexLink}[3]{\special{dvisvgm:raw <a class="cortex-link" xlink:href="\string##/a/#1/#2">}#3\special{dvisvgm:raw </a>}}
\else
	\newcommand{\CortexLinkText}[1]{\special{color push rgb 0.1 0.37 0.7}#1\special{color pop}}
	\newcommand{\CortexUnresolvedLink}[1]{\special{color push rgb 0.75 0.1 0.15}#1\special{color pop}}
	\newcommand{\CortexLink}[3]{%
		\ifcsname cortex@target@#1@#2\endcsname
			\special{pdf:bann << /Type/Annot /Subtype/Link /Border[0 0 0] /A << /S/GoToR /F <\csname cortex@target@#1@#2\endcsname> /D[0 /Fit] >> >>}%
			\CortexLinkText{#3}%
			\special{pdf:eann}%
		\else
			\CortexUnresolvedLink{#3}%
		\fi}
\fi

% ![[transclusions]] are rewritten into \CortexTransclude{<origin>}{<slug>}. The engine resolves each one before compiling (from its origin,
% like links) and declares the fragment to \input with \CortexTranscludeTarget, or why it can't be made (no such note; it would loop back on
% itself) with \CortexTranscludeProblem; the problem is shown in place with \CortexTransclusionProblem, which users may \renewcommand.
\newcommand{\CortexTranscludeTarget}[3]{\expandafter\def\csname cortex@transclude@#1@#2\endcsname{#3}}
\newcommand{\CortexTranscludeProblem}[3]{\expandafter\def\csname cortex@transcludeproblem@#1@#2\endcsname{#3}}
\newcommand{\CortexTransclusionProblem}[1]{{\special{color push rgb 0.75 0.1 0.15}[#1]\special{color pop}}}
\newcommand{\CortexTransclude}[2]{%
	\@ifundefined{cortex@transclude@#1@#2}%
		{\@ifundefined{cortex@transcludeproblem@#1@#2}%
			{\CortexTransclusionProblem{cannot transclude #2}}%
			{\CortexTransclusionProblem{\csname cortex@transcludeproblem@#1@#2\endcsname}}}%
		{\input{\csname cortex@transclude@#1@#2\endcsname}}}
