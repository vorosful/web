---
name: conferences
layout: newbase
years:
- 2025
- 2024
- 2023
- 2022
- 2021
- 2020
- 2019
- 2018
- 2017
- 2016
- 2015
- 2014
---
{% include layouts/find_title.md name=page.name %}

The tables below contain three columns:

* Conference title, which is also a link to the conference webpage
* PHENIX presentations committed to Zenodo (as a link to the relevant Zenodo query page)
* The official list of PHENIX authors approved for this specific conference (optional)

If a presentation link does not produce a result this is an indication that the upload
of the materials is pending. Thank you for your patience!

{{ site.hr }}

{% for year in page.years %}
{% assign c4y=site.data.conferences | where_exp: "item", "item.year==year" | sort: "title" %}
<h5>{{ year }}</h5>
{% for conference in c4y %}
{% capture link %}{%- include navigation/zenodo_query.md name=conference.name tag='Presentations' -%}{% endcapture %}

<table width="100%">
  <tr>
    <td width="60%"><nobr><a href="{{ conference.url }}" target="_blank">{{ conference.title }}</a></nobr></td>
    <td width="30%"><nobr>{{ link }}</nobr></td>
    {% if conference.authors %}
    <td width="10%"><nobr><a href="{{ conference.authors }}" target="_blank">Author list</a></nobr></td>
    {% else %}
    <td width="10%"> - </td>
    {% endif %}
  </tr>
</table>
{% endfor %}
{{ site.hr }}
{% endfor %}
