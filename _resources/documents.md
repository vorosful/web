---
title: Documents
abbrev: documents
name: documents
layout: newbase
weight: 10
items:
   - {title: 'General Overviews',			category: 'overview',	type: 'document'}
   - {title: 'Detector Subsystems (Writeups)',		category: 'detector',	type: 'writeup'}
   - {title: 'Select Theses',				category: 'physics',	type: 'thesis'}
   - {title: 'Misc Summaries',				category: 'summary', div: yes }
---
{% include title.md %}

{% for item in page.items %}
{% if item.div %}<hr/>{% endif %}
{% include documents/doc.md title=item.title category=item.category type=item.type %}
{% endfor %}
